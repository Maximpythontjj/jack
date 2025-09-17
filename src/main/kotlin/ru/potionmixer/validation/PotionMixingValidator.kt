package ru.potionmixer.validation

import org.bukkit.Material
import org.bukkit.inventory.ItemStack
import org.bukkit.inventory.meta.PotionMeta
import org.bukkit.potion.PotionEffect
import org.bukkit.potion.PotionEffectType
import ru.potionmixer.config.ConfigManager
import ru.potionmixer.config.MessageManager

class PotionMixingValidator(
    private val configManager: ConfigManager,
    private val messageManager: MessageManager
) {
    
    data class ValidationResult(
        val isValid: Boolean,
        val reason: String? = null,
        val resultPotion: ItemStack? = null
    )
    
    fun validateAndMix(leftPotion: ItemStack?, rightPotion: ItemStack?): ValidationResult {
        // Проверка на пустые слоты
        if (leftPotion == null || rightPotion == null || 
            leftPotion.type == Material.AIR || rightPotion.type == Material.AIR) {
            return ValidationResult(false, messageManager.getEmptyInputReason())
        }
        
        // Проверка что это зелья
        if (!isPotionItem(leftPotion) || !isPotionItem(rightPotion)) {
            return ValidationResult(false, messageManager.getNotAPotionReason())
        }
        
        // Проверка разрешенных типов
        val allowedBases = configManager.getAllowedBases()
        if (leftPotion.type !in allowedBases || rightPotion.type !in allowedBases) {
            return ValidationResult(false, messageManager.getNotAllowedTypeReason())
        }
        
        // Проверка что зелья не одинаковые
        if (arePotionsIdentical(leftPotion, rightPotion)) {
            return ValidationResult(false, messageManager.getSamePotionsReason())
        }
        
        val leftMeta = leftPotion.itemMeta as PotionMeta
        val rightMeta = rightPotion.itemMeta as PotionMeta
        
        val leftEffects = getAllEffects(leftMeta)
        val rightEffects = getAllEffects(rightMeta)
        
        // Проверка кастомных эффектов
        if (!configManager.allowCustomEffects()) {
            val hasCustomEffects = (leftEffects + rightEffects).any { effect ->
                !isVanillaEffect(effect.type)
            }
            if (hasCustomEffects) {
                return ValidationResult(false, messageManager.getCustomEffectsDisabledReason())
            }
        }
        
        // Проверка совместимости эффектов
        val compatibilityResult = checkEffectCompatibility(leftEffects, rightEffects)
        if (!compatibilityResult.isCompatible) {
            return ValidationResult(false, compatibilityResult.reason)
        }
        
        // Создание результирующего зелья
        val resultPotion = createResultPotion(leftPotion, rightPotion, leftEffects, rightEffects)
        
        return ValidationResult(true, null, resultPotion)
    }
    
    private fun isPotionItem(item: ItemStack): Boolean {
        return item.type in listOf(Material.POTION, Material.SPLASH_POTION, Material.LINGERING_POTION) &&
                item.itemMeta is PotionMeta
    }
    
    private fun arePotionsIdentical(left: ItemStack, right: ItemStack): Boolean {
        if (left.type != right.type) return false
        
        val leftMeta = left.itemMeta as PotionMeta
        val rightMeta = right.itemMeta as PotionMeta
        
        val leftEffects = getAllEffects(leftMeta).sortedBy { it.type.key.toString() }
        val rightEffects = getAllEffects(rightMeta).sortedBy { it.type.key.toString() }
        
        return leftEffects == rightEffects
    }
    
    private fun getAllEffects(potionMeta: PotionMeta): List<PotionEffect> {
        val effects = mutableListOf<PotionEffect>()
        
        // Добавляем базовый эффект зелья (если есть)
        try {
            val basePotionType = potionMeta.basePotionType
            if (basePotionType != null) {
                val baseEffects = basePotionType.potionEffects
                effects.addAll(baseEffects)
            }
        } catch (e: Exception) {
            // Fallback для старых версий или если basePotionType недоступен
        }
        
        // Добавляем кастомные эффекты
        effects.addAll(potionMeta.customEffects)
        
        return effects
    }
    
    private fun isVanillaEffect(effectType: PotionEffectType): Boolean {
        // Список всех ванильных эффектов Minecraft
        val vanillaEffects = setOf(
            PotionEffectType.SPEED, PotionEffectType.SLOWNESS, PotionEffectType.HASTE,
            PotionEffectType.MINING_FATIGUE, PotionEffectType.STRENGTH, PotionEffectType.INSTANT_HEALTH,
            PotionEffectType.INSTANT_DAMAGE, PotionEffectType.JUMP_BOOST, PotionEffectType.NAUSEA,
            PotionEffectType.REGENERATION, PotionEffectType.RESISTANCE, PotionEffectType.FIRE_RESISTANCE,
            PotionEffectType.WATER_BREATHING, PotionEffectType.INVISIBILITY, PotionEffectType.BLINDNESS,
            PotionEffectType.NIGHT_VISION, PotionEffectType.HUNGER, PotionEffectType.WEAKNESS,
            PotionEffectType.POISON, PotionEffectType.WITHER, PotionEffectType.HEALTH_BOOST,
            PotionEffectType.ABSORPTION, PotionEffectType.SATURATION, PotionEffectType.GLOWING,
            PotionEffectType.LEVITATION, PotionEffectType.LUCK, PotionEffectType.UNLUCK,
            PotionEffectType.SLOW_FALLING, PotionEffectType.CONDUIT_POWER, PotionEffectType.DOLPHINS_GRACE,
            PotionEffectType.BAD_OMEN, PotionEffectType.HERO_OF_THE_VILLAGE
        )
        
        return effectType in vanillaEffects
    }
    
    private fun checkEffectCompatibility(
        leftEffects: List<PotionEffect>, 
        rightEffects: List<PotionEffect>
    ): CompatibilityResult {
        val allEffects = leftEffects + rightEffects
        val effectTypeMap = mutableMapOf<PotionEffectType, MutableList<PotionEffect>>()
        
        // Группируем эффекты по типу
        for (effect in allEffects) {
            effectTypeMap.computeIfAbsent(effect.type) { mutableListOf() }.add(effect)
        }
        
        // Проверяем лимит эффектов
        val maxEffects = configManager.getMaxEffects()
        if (effectTypeMap.size > maxEffects) {
            return CompatibilityResult(
                false, 
                messageManager.getTooManyEffectsReason().replace("{max}", maxEffects.toString())
            )
        }
        
        // Проверяем совместимость одинаковых эффектов
        for ((effectType, effects) in effectTypeMap) {
            if (effects.size > 1) {
                val firstEffect = effects[0]
                
                // Проверка одинаковой длительности
                if (configManager.requireSameDuration()) {
                    if (effects.any { it.duration != firstEffect.duration }) {
                        return CompatibilityResult(false, messageManager.getDurationMismatchReason())
                    }
                }
                
                // Проверка одинакового уровня усиления
                if (configManager.requireSameAmplifier()) {
                    if (effects.any { it.amplifier != firstEffect.amplifier }) {
                        return CompatibilityResult(false, messageManager.getAmplifierMismatchReason())
                    }
                }
            }
        }
        
        return CompatibilityResult(true)
    }
    
    private fun createResultPotion(
        leftPotion: ItemStack,
        rightPotion: ItemStack,
        leftEffects: List<PotionEffect>,
        rightEffects: List<PotionEffect>
    ): ItemStack {
        // Определяем тип результирующего зелья
        val resultType = when (configManager.getResultBasis()) {
            ConfigManager.ResultBasisType.LEFT -> leftPotion.type
            ConfigManager.ResultBasisType.RIGHT -> rightPotion.type
            ConfigManager.ResultBasisType.POTION -> Material.POTION
            ConfigManager.ResultBasisType.SPLASH_POTION -> Material.SPLASH_POTION
            ConfigManager.ResultBasisType.LINGERING_POTION -> Material.LINGERING_POTION
        }
        
        val resultPotion = ItemStack(resultType, 1)
        val resultMeta = resultPotion.itemMeta as PotionMeta
        
        // Объединяем эффекты (убираем дубликаты)
        val uniqueEffects = mutableMapOf<PotionEffectType, PotionEffect>()
        for (effect in leftEffects + rightEffects) {
            uniqueEffects[effect.type] = effect
        }
        
        // Добавляем все эффекты как кастомные (чтобы точно контролировать результат)
        for (effect in uniqueEffects.values) {
            resultMeta.addCustomEffect(effect, true)
        }
        
        // Копируем название если разрешено
        if (configManager.allowCustomNames()) {
            val leftMeta = leftPotion.itemMeta
            if (leftMeta?.hasDisplayName() == true) {
                resultMeta.setDisplayName(leftMeta.displayName)
            }
        }
        
        resultPotion.itemMeta = resultMeta
        return resultPotion
    }
    
    private data class CompatibilityResult(
        val isCompatible: Boolean,
        val reason: String? = null
    )
}
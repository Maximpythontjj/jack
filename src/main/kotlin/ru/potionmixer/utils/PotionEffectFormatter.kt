package ru.potionmixer.utils

import org.bukkit.potion.PotionEffect
import org.bukkit.potion.PotionEffectType
import ru.potionmixer.config.MessageManager

object PotionEffectFormatter {
    
    fun formatEffect(effect: PotionEffect, messageManager: MessageManager): String {
        val effectName = getEffectDisplayName(effect.type, messageManager)
        val level = getRomanNumeral(effect.amplifier + 1)
        val duration = formatDuration(effect.duration)
        
        return messageManager.getMessage(
            "effects.format",
            "effect" to effectName,
            "level" to level,
            "duration" to duration
        )
    }
    
    fun formatEffectList(effects: List<PotionEffect>, messageManager: MessageManager): List<String> {
        return effects.map { effect ->
            formatEffect(effect, messageManager)
        }
    }
    
    private fun getEffectDisplayName(effectType: PotionEffectType, messageManager: MessageManager): String {
        val key = "effects.names.${effectType.key.key.uppercase()}"
        val localizedName = messageManager.getMessage(key)
        
        // Если локализованное имя не найдено, используем стандартное имя
        return if (localizedName.contains("Сообщение не найдено")) {
            effectType.key.key.replace("_", " ").lowercase().replaceFirstChar { it.uppercase() }
        } else {
            localizedName
        }
    }
    
    private fun formatDuration(ticks: Int): String {
        if (ticks == Int.MAX_VALUE || ticks < 0) {
            return "∞"
        }
        
        val seconds = ticks / 20
        val minutes = seconds / 60
        val remainingSeconds = seconds % 60
        
        return String.format("%d:%02d", minutes, remainingSeconds)
    }
    
    private fun getRomanNumeral(number: Int): String {
        return when (number) {
            1 -> "I"
            2 -> "II"
            3 -> "III"
            4 -> "IV"
            5 -> "V"
            6 -> "VI"
            7 -> "VII"
            8 -> "VIII"
            9 -> "IX"
            10 -> "X"
            else -> number.toString()
        }
    }
}
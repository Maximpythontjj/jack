package ru.potionmixer.config

import org.bukkit.Material
import org.bukkit.configuration.file.FileConfiguration
import ru.potionmixer.PotionMixerPlugin
import java.util.logging.Level

class ConfigManager(private val plugin: PotionMixerPlugin) {
    
    private lateinit var config: FileConfiguration
    
    fun loadConfig() {
        plugin.saveDefaultConfig()
        plugin.reloadConfig()
        config = plugin.config
        
        // Проверка и обновление конфигурации при необходимости
        validateAndUpdateConfig()
    }
    
    private fun validateAndUpdateConfig() {
        var needsSave = false
        
        // Проверяем наличие всех необходимых ключей
        val defaults = mapOf(
            "max-effects" to 5,
            "require-same-duration" to true,
            "require-same-amplifier" to true,
            "allowed-bases" to listOf("POTION", "SPLASH_POTION", "LINGERING_POTION"),
            "result-basis" to "LEFT",
            "open-on-right-click" to true,
            "right-click-requires-empty-hand" to false,
            "allow-custom-names" to false,
            "allow-custom-effects" to false,
            "sound-enabled" to true,
            "particles-enabled" to true,
            "debug-logs" to false,
            "commands.enabled" to true,
            "commands.cauldron-command" to true
        )
        
        for ((key, defaultValue) in defaults) {
            if (!config.contains(key)) {
                config.set(key, defaultValue)
                needsSave = true
            }
        }
        
        if (needsSave) {
            plugin.saveConfig()
            plugin.logger.info("Конфигурация обновлена с новыми настройками")
        }
    }
    
    fun getMaxEffects(): Int = config.getInt("max-effects", 5)
    
    fun requireSameDuration(): Boolean = config.getBoolean("require-same-duration", true)
    
    fun requireSameAmplifier(): Boolean = config.getBoolean("require-same-amplifier", true)
    
    fun getAllowedBases(): List<Material> {
        val bases = config.getStringList("allowed-bases")
        return bases.mapNotNull { baseName ->
            try {
                Material.valueOf(baseName.uppercase())
            } catch (e: IllegalArgumentException) {
                plugin.logger.warning("Неизвестный тип зелья в конфигурации: $baseName")
                null
            }
        }
    }
    
    fun getResultBasis(): ResultBasisType {
        val basis = config.getString("result-basis", "LEFT")?.uppercase()
        return try {
            when (basis) {
                "LEFT" -> ResultBasisType.LEFT
                "RIGHT" -> ResultBasisType.RIGHT
                "POTION" -> ResultBasisType.POTION
                "SPLASH_POTION" -> ResultBasisType.SPLASH_POTION
                "LINGERING_POTION" -> ResultBasisType.LINGERING_POTION
                else -> {
                    plugin.logger.warning("Неизвестный тип result-basis: $basis, используется LEFT")
                    ResultBasisType.LEFT
                }
            }
        } catch (e: Exception) {
            plugin.logger.log(Level.WARNING, "Ошибка при парсинге result-basis", e)
            ResultBasisType.LEFT
        }
    }
    
    fun openOnRightClick(): Boolean = config.getBoolean("open-on-right-click", true)
    
    fun rightClickRequiresEmptyHand(): Boolean = config.getBoolean("right-click-requires-empty-hand", false)
    
    fun allowCustomNames(): Boolean = config.getBoolean("allow-custom-names", false)
    
    fun allowCustomEffects(): Boolean = config.getBoolean("allow-custom-effects", false)
    
    fun isSoundEnabled(): Boolean = config.getBoolean("sound-enabled", true)
    
    fun isParticlesEnabled(): Boolean = config.getBoolean("particles-enabled", true)
    
    fun isDebugEnabled(): Boolean = config.getBoolean("debug-logs", false)
    
    fun areCommandsEnabled(): Boolean = config.getBoolean("commands.enabled", true)
    
    fun isCauldronCommandEnabled(): Boolean = config.getBoolean("commands.cauldron-command", true)
    
    enum class ResultBasisType {
        LEFT, RIGHT, POTION, SPLASH_POTION, LINGERING_POTION
    }
}
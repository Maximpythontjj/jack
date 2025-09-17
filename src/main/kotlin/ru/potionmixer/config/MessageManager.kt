package ru.potionmixer.config

import org.bukkit.configuration.file.FileConfiguration
import org.bukkit.configuration.file.YamlConfiguration
import ru.potionmixer.PotionMixerPlugin
import java.io.File
import java.io.InputStreamReader
import java.nio.charset.StandardCharsets
import java.util.logging.Level

class MessageManager(private val plugin: PotionMixerPlugin) {
    
    private lateinit var messagesConfig: FileConfiguration
    private lateinit var messagesFile: File
    
    fun loadMessages() {
        messagesFile = File(plugin.dataFolder, "messages.yml")
        
        if (!messagesFile.exists()) {
            plugin.saveResource("messages.yml", false)
        }
        
        messagesConfig = YamlConfiguration.loadConfiguration(messagesFile)
        
        // Загрузка дефолтных значений из ресурсов
        val defaultMessages = plugin.getResource("messages.yml")?.let { resource ->
            YamlConfiguration.loadConfiguration(InputStreamReader(resource, StandardCharsets.UTF_8))
        }
        
        if (defaultMessages != null) {
            messagesConfig.setDefaults(defaultMessages)
            
            // Добавляем недостающие ключи
            var needsSave = false
            for (key in defaultMessages.getKeys(true)) {
                if (!messagesConfig.contains(key)) {
                    messagesConfig.set(key, defaultMessages.get(key))
                    needsSave = true
                }
            }
            
            if (needsSave) {
                saveMessagesConfig()
                plugin.logger.info("Файл сообщений обновлен с новыми ключами")
            }
        }
    }
    
    private fun saveMessagesConfig() {
        try {
            messagesConfig.save(messagesFile)
        } catch (e: Exception) {
            plugin.logger.log(Level.SEVERE, "Не удалось сохранить файл сообщений", e)
        }
    }
    
    fun getMessage(key: String, vararg placeholders: Pair<String, String>): String {
        var message = messagesConfig.getString(key) ?: "Сообщение не найдено: $key"
        
        // Замена плейсхолдеров
        for ((placeholder, value) in placeholders) {
            message = message.replace("{$placeholder}", value)
        }
        
        return message.replace("&", "§") // Цветовые коды
    }
    
    fun getTitle(): String = getMessage("gui.title")
    
    fun getResultUnavailable(reason: String): String = getMessage("gui.result-unavailable", "reason" to reason)
    
    fun getSuccessMessage(): String = getMessage("messages.success")
    
    fun getNoPermissionMessage(): String = getMessage("messages.no-permission")
    
    fun getReloadedMessage(): String = getMessage("messages.reloaded")
    
    fun getCommandsDisabledMessage(): String = getMessage("messages.commands-disabled")
    
    // Причины отказа смешивания
    fun getNotAPotionReason(): String = getMessage("mixing.not-a-potion")
    
    fun getDurationMismatchReason(): String = getMessage("mixing.duration-mismatch")
    
    fun getAmplifierMismatchReason(): String = getMessage("mixing.amplifier-mismatch")
    
    fun getTooManyEffectsReason(): String = getMessage("mixing.too-many-effects")
    
    fun getNotAllowedTypeReason(): String = getMessage("mixing.not-allowed-type")
    
    fun getCustomEffectsDisabledReason(): String = getMessage("mixing.custom-effects-disabled")
    
    fun getEmptyInputReason(): String = getMessage("mixing.empty-input")
    
    fun getSamePotionsReason(): String = getMessage("mixing.same-potions")
}
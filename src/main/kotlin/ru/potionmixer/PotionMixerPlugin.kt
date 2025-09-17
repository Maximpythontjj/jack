package ru.potionmixer

import org.bukkit.plugin.java.JavaPlugin
import ru.potionmixer.commands.CauldronCommand
import ru.potionmixer.commands.PotionMixerCommand
import ru.potionmixer.config.ConfigManager
import ru.potionmixer.config.MessageManager
import ru.potionmixer.gui.CauldronGUIManager
import ru.potionmixer.listeners.CauldronInteractionListener
import ru.potionmixer.listeners.InventoryListener
import ru.potionmixer.validation.PotionMixingValidator

class PotionMixerPlugin : JavaPlugin() {
    
    lateinit var configManager: ConfigManager
        private set
    
    lateinit var messageManager: MessageManager
        private set
    
    lateinit var guiManager: CauldronGUIManager
        private set
    
    lateinit var mixingValidator: PotionMixingValidator
        private set
    
    override fun onEnable() {
        // Инициализация менеджеров
        configManager = ConfigManager(this)
        messageManager = MessageManager(this)
        mixingValidator = PotionMixingValidator(configManager, messageManager)
        guiManager = CauldronGUIManager(this, mixingValidator, messageManager)
        
        // Загрузка конфигураций
        configManager.loadConfig()
        messageManager.loadMessages()
        
        // Регистрация слушателей событий
        registerListeners()
        
        // Регистрация команд
        registerCommands()
        
        logger.info("PotionMixer v${description.version} успешно загружен!")
        
        if (configManager.isDebugEnabled()) {
            logger.info("Режим отладки включен")
        }
    }
    
    override fun onDisable() {
        // Закрытие всех открытых GUI
        guiManager.closeAllGUIs()
        
        logger.info("PotionMixer выключен!")
    }
    
    private fun registerListeners() {
        val pluginManager = server.pluginManager
        
        pluginManager.registerEvents(CauldronInteractionListener(this), this)
        pluginManager.registerEvents(InventoryListener(this), this)
    }
    
    private fun registerCommands() {
        getCommand("potionmixer")?.setExecutor(PotionMixerCommand(this))
        getCommand("cauldron")?.setExecutor(CauldronCommand(this))
    }
    
    fun reloadConfigs() {
        configManager.loadConfig()
        messageManager.loadMessages()
        mixingValidator = PotionMixingValidator(configManager, messageManager)
        guiManager.updateValidator(mixingValidator)
        
        if (configManager.isDebugEnabled()) {
            logger.info("Конфигурация перезагружена")
        }
    }
}
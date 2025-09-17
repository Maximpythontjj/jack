package ru.potionmixer.commands

import org.bukkit.command.Command
import org.bukkit.command.CommandExecutor
import org.bukkit.command.CommandSender
import org.bukkit.command.TabCompleter
import org.bukkit.entity.Player
import ru.potionmixer.PotionMixerPlugin

class PotionMixerCommand(private val plugin: PotionMixerPlugin) : CommandExecutor, TabCompleter {
    
    override fun onCommand(sender: CommandSender, command: Command, label: String, args: Array<String>): Boolean {
        if (!plugin.configManager.areCommandsEnabled()) {
            sender.sendMessage(plugin.messageManager.getCommandsDisabledMessage())
            return true
        }
        
        if (args.isEmpty()) {
            sendHelpMessage(sender)
            return true
        }
        
        when (args[0].lowercase()) {
            "reload" -> {
                if (!sender.hasPermission("potionmixer.admin")) {
                    sender.sendMessage(plugin.messageManager.getNoPermissionMessage())
                    return true
                }
                
                try {
                    plugin.reloadConfigs()
                    sender.sendMessage(plugin.messageManager.getReloadedMessage())
                    
                    if (plugin.configManager.isDebugEnabled()) {
                        plugin.logger.info("Конфигурация перезагружена командой от ${sender.name}")
                    }
                } catch (e: Exception) {
                    sender.sendMessage("§cОшибка при перезагрузке конфигурации: ${e.message}")
                    plugin.logger.severe("Ошибка при перезагрузке конфигурации: ${e.message}")
                }
                
                return true
            }
            
            "help" -> {
                sendHelpMessage(sender)
                return true
            }
            
            "version" -> {
                sender.sendMessage("§6PotionMixer §7v${plugin.description.version}")
                sender.sendMessage("§7Автор: ${plugin.description.authors.joinToString(", ")}")
                return true
            }
            
            "debug" -> {
                if (!sender.hasPermission("potionmixer.admin")) {
                    sender.sendMessage(plugin.messageManager.getNoPermissionMessage())
                    return true
                }
                
                if (sender is Player) {
                    val player = sender
                    sender.sendMessage("§6=== PotionMixer Debug Info ===")
                    sender.sendMessage("§7Версия: ${plugin.description.version}")
                    sender.sendMessage("§7Отладка включена: ${plugin.configManager.isDebugEnabled()}")
                    sender.sendMessage("§7Максимум эффектов: ${plugin.configManager.getMaxEffects()}")
                    sender.sendMessage("§7Разрешенные типы: ${plugin.configManager.getAllowedBases().joinToString(", ")}")
                    sender.sendMessage("§7GUI открыт: ${plugin.guiManager.isPlayerInCauldronGUI(player)}")
                    sender.sendMessage("§7Права:")
                    sender.sendMessage("§8  - potionmixer.use: ${player.hasPermission("potionmixer.use")}")
                    sender.sendMessage("§8  - potionmixer.admin: ${player.hasPermission("potionmixer.admin")}")
                    sender.sendMessage("§8  - potionmixer.bypass.limit: ${player.hasPermission("potionmixer.bypass.limit")}")
                }
                
                return true
            }
            
            else -> {
                sender.sendMessage("§cНеизвестная подкоманда: ${args[0]}")
                sendHelpMessage(sender)
                return true
            }
        }
    }
    
    private fun sendHelpMessage(sender: CommandSender) {
        sender.sendMessage("§6=== PotionMixer Commands ===")
        sender.sendMessage("§e/potionmixer help §7- Показать эту справку")
        sender.sendMessage("§e/potionmixer version §7- Информация о версии")
        
        if (sender.hasPermission("potionmixer.admin")) {
            sender.sendMessage("§e/potionmixer reload §7- Перезагрузить конфигурацию")
            sender.sendMessage("§e/potionmixer debug §7- Показать отладочную информацию")
        }
        
        if (plugin.configManager.isCauldronCommandEnabled()) {
            sender.sendMessage("§e/cauldron §7- Открыть интерфейс котла")
        }
    }
    
    override fun onTabComplete(sender: CommandSender, command: Command, label: String, args: Array<String>): List<String> {
        if (args.size == 1) {
            val subcommands = mutableListOf("help", "version")
            
            if (sender.hasPermission("potionmixer.admin")) {
                subcommands.addAll(listOf("reload", "debug"))
            }
            
            return subcommands.filter { it.startsWith(args[0].lowercase()) }
        }
        
        return emptyList()
    }
}
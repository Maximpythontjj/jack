package ru.potionmixer.commands

import org.bukkit.command.Command
import org.bukkit.command.CommandExecutor
import org.bukkit.command.CommandSender
import org.bukkit.entity.Player
import ru.potionmixer.PotionMixerPlugin

class CauldronCommand(private val plugin: PotionMixerPlugin) : CommandExecutor {
    
    override fun onCommand(sender: CommandSender, command: Command, label: String, args: Array<String>): Boolean {
        if (!plugin.configManager.areCommandsEnabled()) {
            sender.sendMessage(plugin.messageManager.getCommandsDisabledMessage())
            return true
        }
        
        if (!plugin.configManager.isCauldronCommandEnabled()) {
            sender.sendMessage("§cКоманда /cauldron отключена в конфигурации")
            return true
        }
        
        if (sender !is Player) {
            sender.sendMessage("§cЭта команда доступна только игрокам!")
            return true
        }
        
        val player = sender
        
        if (!player.hasPermission("potionmixer.use")) {
            player.sendMessage(plugin.messageManager.getNoPermissionMessage())
            return true
        }
        
        // Открываем GUI котла
        plugin.guiManager.openCauldronGUI(player)
        
        if (plugin.configManager.isDebugEnabled()) {
            plugin.logger.info("Игрок ${player.name} открыл GUI котла через команду")
        }
        
        return true
    }
}
package ru.potionmixer.listeners

import org.bukkit.Material
import org.bukkit.block.Block
import org.bukkit.entity.Player
import org.bukkit.event.EventHandler
import org.bukkit.event.EventPriority
import org.bukkit.event.Listener
import org.bukkit.event.block.Action
import org.bukkit.event.player.PlayerInteractEvent
import ru.potionmixer.PotionMixerPlugin

class CauldronInteractionListener(private val plugin: PotionMixerPlugin) : Listener {
    
    @EventHandler(priority = EventPriority.NORMAL)
    fun onPlayerInteract(event: PlayerInteractEvent) {
        if (event.action != Action.RIGHT_CLICK_BLOCK) {
            return
        }
        
        val block = event.clickedBlock ?: return
        if (!isCauldron(block)) {
            return
        }
        
        val player = event.player
        
        // Проверяем настройки открытия GUI
        if (!plugin.configManager.openOnRightClick()) {
            return
        }
        
        // Проверяем требование пустой руки
        if (plugin.configManager.rightClickRequiresEmptyHand()) {
            val itemInHand = player.inventory.itemInMainHand
            if (itemInHand.type != Material.AIR) {
                return
            }
        }
        
        // Проверяем права доступа
        if (!player.hasPermission("potionmixer.use")) {
            player.sendMessage(plugin.messageManager.getNoPermissionMessage())
            event.isCancelled = true
            return
        }
        
        // Отменяем стандартное взаимодействие с котлом
        event.isCancelled = true
        
        // Открываем GUI
        plugin.guiManager.openCauldronGUI(player)
        
        if (plugin.configManager.isDebugEnabled()) {
            plugin.logger.info("Игрок ${player.name} взаимодействует с котлом в ${block.location}")
        }
    }
    
    private fun isCauldron(block: Block): Boolean {
        return block.type in listOf(
            Material.CAULDRON,
            Material.WATER_CAULDRON,
            Material.LAVA_CAULDRON,
            Material.POWDER_SNOW_CAULDRON
        )
    }
}
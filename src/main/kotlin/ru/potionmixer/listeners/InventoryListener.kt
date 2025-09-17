package ru.potionmixer.listeners

import org.bukkit.entity.Player
import org.bukkit.event.EventHandler
import org.bukkit.event.EventPriority
import org.bukkit.event.Listener
import org.bukkit.event.inventory.InventoryClickEvent
import org.bukkit.event.inventory.InventoryCloseEvent
import org.bukkit.event.inventory.InventoryDragEvent
import ru.potionmixer.PotionMixerPlugin
import ru.potionmixer.gui.CauldronGUIManager

class InventoryListener(private val plugin: PotionMixerPlugin) : Listener {
    
    @EventHandler(priority = EventPriority.NORMAL)
    fun onInventoryClick(event: InventoryClickEvent) {
        if (event.whoClicked !is Player) {
            return
        }
        
        val player = event.whoClicked as Player
        val inventory = event.inventory
        
        // Проверяем что это наш GUI
        if (inventory.holder !is CauldronGUIManager) {
            return
        }
        
        val slot = event.rawSlot
        val clickedItem = event.currentItem
        val cursor = event.cursor
        
        val shouldCancel = plugin.guiManager.handleInventoryClick(player, inventory, slot, clickedItem, cursor)
        
        if (shouldCancel) {
            event.isCancelled = true
        }
        
        if (plugin.configManager.isDebugEnabled()) {
            plugin.logger.info("Игрок ${player.name} кликнул в слот $slot, отменено: $shouldCancel")
        }
    }
    
    @EventHandler(priority = EventPriority.NORMAL)
    fun onInventoryDrag(event: InventoryDragEvent) {
        if (event.whoClicked !is Player) {
            return
        }
        
        val inventory = event.inventory
        
        // Проверяем что это наш GUI
        if (inventory.holder !is CauldronGUIManager) {
            return
        }
        
        // Проверяем затрагивает ли перетаскивание наши слоты
        val affectedSlots = event.rawSlots
        val restrictedSlots = CauldronGUIManager.DECORATION_SLOTS + CauldronGUIManager.RESULT_SLOT
        
        val shouldCancel = affectedSlots.any { slot -> 
            slot < inventory.size && slot in restrictedSlots 
        }
        
        if (shouldCancel) {
            event.isCancelled = true
            
            if (plugin.configManager.isDebugEnabled()) {
                val player = event.whoClicked as Player
                plugin.logger.info("Перетаскивание игрока ${player.name} заблокировано")
            }
        }
    }
    
    @EventHandler(priority = EventPriority.NORMAL)
    fun onInventoryClose(event: InventoryCloseEvent) {
        if (event.player !is Player) {
            return
        }
        
        val player = event.player as Player
        val inventory = event.inventory
        
        // Проверяем что это наш GUI
        if (inventory.holder !is CauldronGUIManager) {
            return
        }
        
        plugin.guiManager.handleInventoryClose(player, inventory)
    }
}
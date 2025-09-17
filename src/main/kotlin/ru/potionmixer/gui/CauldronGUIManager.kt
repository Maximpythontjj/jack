package ru.potionmixer.gui

import org.bukkit.Bukkit
import org.bukkit.Material
import org.bukkit.Particle
import org.bukkit.Sound
import org.bukkit.entity.Player
import org.bukkit.inventory.Inventory
import org.bukkit.inventory.InventoryHolder
import org.bukkit.inventory.ItemStack
import org.bukkit.inventory.meta.ItemMeta
import ru.potionmixer.PotionMixerPlugin
import ru.potionmixer.config.MessageManager
import ru.potionmixer.validation.PotionMixingValidator
import java.util.*

class CauldronGUIManager(
    private val plugin: PotionMixerPlugin,
    private var mixingValidator: PotionMixingValidator,
    private val messageManager: MessageManager
) : InventoryHolder {
    
    companion object {
        const val INVENTORY_SIZE = 27
        const val LEFT_SLOT = 10
        const val RIGHT_SLOT = 12
        const val RESULT_SLOT = 16
        
        // Слоты для декоративных предметов
        val DECORATION_SLOTS = listOf(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 13, 14, 15, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26)
    }
    
    private val openGUIs = mutableMapOf<UUID, Inventory>()
    
    override fun getInventory(): Inventory {
        return Bukkit.createInventory(this, INVENTORY_SIZE, "Котёл")
    }
    
    fun updateValidator(newValidator: PotionMixingValidator) {
        mixingValidator = newValidator
    }
    
    fun openCauldronGUI(player: Player) {
        if (!player.hasPermission("potionmixer.use")) {
            player.sendMessage(messageManager.getNoPermissionMessage())
            return
        }
        
        val inventory = Bukkit.createInventory(this, INVENTORY_SIZE, messageManager.getTitle())
        
        // Заполняем декоративными предметами
        fillDecorationSlots(inventory)
        
        // Обновляем результат (пустой при открытии)
        updateResult(inventory)
        
        openGUIs[player.uniqueId] = inventory
        player.openInventory(inventory)
        
        if (plugin.configManager.isDebugEnabled()) {
            plugin.logger.info("Игрок ${player.name} открыл GUI котла")
        }
    }
    
    private fun fillDecorationSlots(inventory: Inventory) {
        val glassPane = ItemStack(Material.GRAY_STAINED_GLASS_PANE)
        val glassMeta = glassPane.itemMeta
        glassMeta?.setDisplayName(" ")
        glassPane.itemMeta = glassMeta
        
        for (slot in DECORATION_SLOTS) {
            inventory.setItem(slot, glassPane)
        }
        
        // Специальная панель для результата, если он недоступен
        val resultPane = ItemStack(Material.RED_STAINED_GLASS_PANE)
        val resultMeta = resultPane.itemMeta
        resultMeta?.setDisplayName(" ")
        resultPane.itemMeta = resultMeta
        
        // Пока что ставим красную панель в результат
        inventory.setItem(RESULT_SLOT, resultPane)
    }
    
    fun handleInventoryClick(player: Player, inventory: Inventory, slot: Int, clickedItem: ItemStack?, cursor: ItemStack?): Boolean {
        if (!isValidCauldronGUI(player, inventory)) {
            return false
        }
        
        when (slot) {
            LEFT_SLOT, RIGHT_SLOT -> {
                // Разрешаем взаимодействие с входными слотами
                Bukkit.getScheduler().runTaskLater(plugin, Runnable {
                    updateResult(inventory)
                }, 1L)
                return false // Позволяем стандартное поведение
            }
            RESULT_SLOT -> {
                return handleResultSlotClick(player, inventory, cursor)
            }
            in DECORATION_SLOTS -> {
                return true // Блокируем клики по декоративным слотам
            }
            else -> {
                return false // Разрешаем взаимодействие с инвентарем игрока
            }
        }
    }
    
    private fun handleResultSlotClick(player: Player, inventory: Inventory, cursor: ItemStack?): Boolean {
        if (cursor != null && cursor.type != Material.AIR) {
            return true // Блокируем если у игрока что-то в курсоре
        }
        
        val leftItem = inventory.getItem(LEFT_SLOT)
        val rightItem = inventory.getItem(RIGHT_SLOT)
        val resultItem = inventory.getItem(RESULT_SLOT)
        
        // Проверяем что результат существует и это не декоративная панель
        if (resultItem == null || isDecorationItem(resultItem)) {
            return true
        }
        
        // Проверяем валидность смешивания еще раз
        val validationResult = mixingValidator.validateAndMix(leftItem, rightItem)
        if (!validationResult.isValid || validationResult.resultPotion == null) {
            return true
        }
        
        // Выдаем результат игроку
        player.inventory.addItem(validationResult.resultPotion)
        
        // Убираем входные предметы
        inventory.setItem(LEFT_SLOT, null)
        inventory.setItem(RIGHT_SLOT, null)
        
        // Обновляем результат
        updateResult(inventory)
        
        // Эффекты успешного смешивания
        playSuccessEffects(player)
        
        player.sendMessage(messageManager.getSuccessMessage())
        
        if (plugin.configManager.isDebugEnabled()) {
            plugin.logger.info("Игрок ${player.name} успешно смешал зелья")
        }
        
        return true
    }
    
    private fun updateResult(inventory: Inventory) {
        val leftItem = inventory.getItem(LEFT_SLOT)
        val rightItem = inventory.getItem(RIGHT_SLOT)
        
        val validationResult = mixingValidator.validateAndMix(leftItem, rightItem)
        
        if (validationResult.isValid && validationResult.resultPotion != null) {
            // Устанавливаем результирующее зелье
            inventory.setItem(RESULT_SLOT, validationResult.resultPotion)
        } else {
            // Устанавливаем блокирующий предмет с причиной
            val blockingItem = createBlockingItem(validationResult.reason ?: "Неизвестная ошибка")
            inventory.setItem(RESULT_SLOT, blockingItem)
        }
    }
    
    private fun createBlockingItem(reason: String): ItemStack {
        val item = ItemStack(Material.BARRIER)
        val meta = item.itemMeta
        
        meta?.setDisplayName("§c§lНедоступно")
        meta?.lore = listOf("§7$reason")
        
        item.itemMeta = meta
        return item
    }
    
    private fun isDecorationItem(item: ItemStack): Boolean {
        return item.type in listOf(
            Material.GRAY_STAINED_GLASS_PANE,
            Material.RED_STAINED_GLASS_PANE,
            Material.BARRIER
        ) && (item.itemMeta?.displayName == " " || item.itemMeta?.displayName?.contains("Недоступно") == true)
    }
    
    private fun playSuccessEffects(player: Player) {
        if (plugin.configManager.isSoundEnabled()) {
            player.playSound(player.location, Sound.BLOCK_BREWING_STAND_BREW, 1.0f, 1.2f)
        }
        
        if (plugin.configManager.isParticlesEnabled()) {
            player.spawnParticle(Particle.HAPPY_VILLAGER, player.location.add(0.0, 1.0, 0.0), 10, 0.5, 0.5, 0.5, 0.1)
        }
    }
    
    fun handleInventoryClose(player: Player, inventory: Inventory) {
        if (!isValidCauldronGUI(player, inventory)) {
            return
        }
        
        // Возвращаем предметы игроку
        val leftItem = inventory.getItem(LEFT_SLOT)
        val rightItem = inventory.getItem(RIGHT_SLOT)
        
        if (leftItem != null && leftItem.type != Material.AIR) {
            player.inventory.addItem(leftItem)
        }
        
        if (rightItem != null && rightItem.type != Material.AIR) {
            player.inventory.addItem(rightItem)
        }
        
        openGUIs.remove(player.uniqueId)
        
        if (plugin.configManager.isDebugEnabled()) {
            plugin.logger.info("Игрок ${player.name} закрыл GUI котла")
        }
    }
    
    private fun isValidCauldronGUI(player: Player, inventory: Inventory): Boolean {
        return openGUIs[player.uniqueId] == inventory && inventory.holder == this
    }
    
    fun closeAllGUIs() {
        val playersToClose = openGUIs.keys.toList()
        
        for (playerId in playersToClose) {
            val player = Bukkit.getPlayer(playerId)
            if (player != null && player.isOnline) {
                player.closeInventory()
            }
        }
        
        openGUIs.clear()
    }
    
    fun isPlayerInCauldronGUI(player: Player): Boolean {
        return openGUIs.containsKey(player.uniqueId)
    }
}
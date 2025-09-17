package io.papermc.paper.datacomponent.item;

import org.bukkit.craftbukkit.util.Handleable;

public record PaperCustomModelData(
    net.minecraft.world.item.component.CustomModelData impl
) implements CustomModelData, Handleable<net.minecraft.world.item.component.CustomModelData> {

    @Override
    public net.minecraft.world.item.component.CustomModelData getHandle() {
        return this.impl;
    }

    @Override
    public int id() {
        return this.impl.value();
    }
}

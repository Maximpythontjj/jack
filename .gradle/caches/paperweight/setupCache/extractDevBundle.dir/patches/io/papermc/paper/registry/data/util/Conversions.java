package io.papermc.paper.registry.data.util;

import com.mojang.serialization.JavaOps;
import io.papermc.paper.adventure.WrapperAwareSerializer;
import net.kyori.adventure.text.Component;
import net.minecraft.resources.RegistryOps;
import org.jetbrains.annotations.Contract;
import org.jspecify.annotations.Nullable;

public class Conversions {

    private final RegistryOps.RegistryInfoLookup lookup;
    private final WrapperAwareSerializer serializer;

    public Conversions(final RegistryOps.RegistryInfoLookup lookup) {
        this.lookup = lookup;
        this.serializer = new WrapperAwareSerializer(() -> RegistryOps.create(JavaOps.INSTANCE, lookup));
    }

    public RegistryOps.RegistryInfoLookup lookup() {
        return this.lookup;
    }

    @Contract("null -> null; !null -> !null")
    public net.minecraft.network.chat.@Nullable Component asVanilla(final @Nullable Component adventure) {
        if (adventure == null) return null;
        return this.serializer.serialize(adventure);
    }

    public Component asAdventure(final net.minecraft.network.chat.@Nullable Component vanilla) {
        return vanilla == null ? Component.empty() : this.serializer.deserialize(vanilla);
    }
}

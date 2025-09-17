import net.minecrell.pluginyml.bukkit.BukkitPluginDescription

plugins {
    kotlin("jvm") version "1.9.22"
    id("net.minecrell.plugin-yml.bukkit") version "0.6.0"
}

group = "ru.potionmixer"
version = "1.0.0"
description = "Advanced potion mixing plugin for Minecraft Paper"

java {
    toolchain.languageVersion.set(JavaLanguageVersion.of(21))
}

repositories {
    mavenCentral()
    maven("https://repo.papermc.io/repository/maven-public/")
    maven("https://oss.sonatype.org/content/groups/public/")
}

dependencies {
    compileOnly("io.papermc.paper:paper-api:1.21.1-R0.1-SNAPSHOT")
    implementation(kotlin("stdlib"))
}

tasks {
    compileKotlin {
        kotlinOptions.jvmTarget = "21"
    }
    
    processResources {
        val props = mapOf("version" to version)
        inputs.properties(props)
        filteringCharset = "UTF-8"
        filesMatching("plugin.yml") {
            expand(props)
        }
    }
    
    jar {
        from(configurations.runtimeClasspath.get().map { if (it.isDirectory) it else zipTree(it) })
        duplicatesStrategy = DuplicatesStrategy.EXCLUDE
    }
}

bukkit {
    load = BukkitPluginDescription.PluginLoadOrder.STARTUP
    main = "ru.potionmixer.PotionMixerPlugin"
    apiVersion = "1.21"
    authors = listOf("PotionMixer Team")
    depend = listOf()
    softDepend = listOf()
    
    commands {
        register("potionmixer") {
            description = "PotionMixer main command"
            aliases = listOf("pmix")
            permission = "potionmixer.admin"
            usage = "/<command> [reload]"
        }
        register("cauldron") {
            description = "Open cauldron GUI directly"
            permission = "potionmixer.use"
            usage = "/<command>"
        }
    }
    
    permissions {
        register("potionmixer.use") {
            description = "Allow using cauldron mixing interface"
            default = BukkitPluginDescription.Permission.Default.TRUE
        }
        register("potionmixer.admin") {
            description = "Allow using admin commands"
            default = BukkitPluginDescription.Permission.Default.OP
        }
        register("potionmixer.bypass.limit") {
            description = "Bypass effect limit when mixing potions"
            default = BukkitPluginDescription.Permission.Default.FALSE
        }
    }
}
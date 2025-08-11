package com.example.app.ui.theme

import androidx.compose.material3.darkColorScheme
import androidx.compose.material3.lightColorScheme
import androidx.compose.ui.graphics.Color

val BrandPrimary = Color(0xFF0E7C66) // teal-green
val BrandOnPrimary = Color(0xFFFFFFFF)
val BrandPrimaryContainer = Color(0xFFB2F1E1)
val BrandOnPrimaryContainer = Color(0xFF002019)

val BrandSecondary = Color(0xFFFFB703) // amber
val BrandOnSecondary = Color(0xFF332200)
val BrandSecondaryContainer = Color(0xFFFFDE9E)
val BrandOnSecondaryContainer = Color(0xFF261900)

val BrandTertiary = Color(0xFF3A86FF) // blue
val BrandOnTertiary = Color(0xFFFFFFFF)

val BrandBackgroundLight = Color(0xFFF8FAFC)
val BrandOnBackgroundLight = Color(0xFF0B1320)
val BrandSurfaceLight = Color(0xFFFFFFFF)
val BrandOnSurfaceLight = Color(0xFF1F2937)
val BrandSurfaceVariantLight = Color(0xFFE5E7EB)
val BrandOnSurfaceVariantLight = Color(0xFF374151)

val BrandError = Color(0xFFB00020)
val BrandOnError = Color(0xFFFFFFFF)
val BrandOutline = Color(0xFF9CA3AF)

val LightColors = lightColorScheme(
    primary = BrandPrimary,
    onPrimary = BrandOnPrimary,
    primaryContainer = BrandPrimaryContainer,
    onPrimaryContainer = BrandOnPrimaryContainer,
    secondary = BrandSecondary,
    onSecondary = BrandOnSecondary,
    secondaryContainer = BrandSecondaryContainer,
    onSecondaryContainer = BrandOnSecondaryContainer,
    tertiary = BrandTertiary,
    onTertiary = BrandOnTertiary,

    background = BrandBackgroundLight,
    onBackground = BrandOnBackgroundLight,
    surface = BrandSurfaceLight,
    onSurface = BrandOnSurfaceLight,
    surfaceVariant = BrandSurfaceVariantLight,
    onSurfaceVariant = BrandOnSurfaceVariantLight,

    error = BrandError,
    onError = BrandOnError,
    outline = BrandOutline,
)

val DarkColors = darkColorScheme(
    primary = Color(0xFF86E8D1),
    onPrimary = Color(0xFF00382D),
    primaryContainer = Color(0xFF005244),
    onPrimaryContainer = Color(0xFFB2F1E1),

    secondary = Color(0xFFFFCF66),
    onSecondary = Color(0xFF3A2A00),
    secondaryContainer = Color(0xFF5C4600),
    onSecondaryContainer = Color(0xFFFFDE9E),

    tertiary = Color(0xFF85B6FF),
    onTertiary = Color(0xFF002E66),

    background = Color(0xFF0B1320),
    onBackground = Color(0xFFE5E7EB),
    surface = Color(0xFF111827),
    onSurface = Color(0xFFE5E7EB),
    surfaceVariant = Color(0xFF374151),
    onSurfaceVariant = Color(0xFFC7CBD1),

    error = BrandError,
    onError = BrandOnError,
    outline = Color(0xFF6B7280),
)

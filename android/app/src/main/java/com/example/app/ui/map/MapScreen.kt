package com.example.app.ui.map

import androidx.compose.foundation.layout.*
import androidx.compose.material3.Button
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.example.app.vm.MapViewModel

@Composable
fun MapScreen(onStartRide: () -> Unit, onProfile: () -> Unit, viewModel: MapViewModel) {
    val bt = viewModel.bluetoothEnabled.collectAsState(initial = false)
    val health = viewModel.health.collectAsState(initial = null)
    Box(modifier = Modifier.fillMaxSize(), contentAlignment = Alignment.Center) {
        Column(horizontalAlignment = Alignment.CenterHorizontally, verticalArrangement = Arrangement.spacedBy(12.dp)) {
            Text("Map Screen (placeholder)")
            Text("Bluetooth enabled: ${'$'}{bt.value}")
            Text("Backend health: ${'$'}{health.value ?: "unknown"}")
            Button(onClick = { viewModel.refreshHealth() }) { Text("Check Health") }
            Button(onClick = onStartRide) { Text("Start Ride") }
            Button(onClick = onProfile) { Text("Profile") }
        }
    }
}
package com.example.app.ui.ride

import androidx.compose.foundation.layout.*
import androidx.compose.material3.Button
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.example.app.vm.RideViewModel

@Composable
fun RideScreen(onBack: () -> Unit, viewModel: RideViewModel) {
    Box(modifier = Modifier.fillMaxSize(), contentAlignment = Alignment.Center) {
        Column(horizontalAlignment = Alignment.CenterHorizontally, verticalArrangement = Arrangement.spacedBy(12.dp)) {
            Text("Ride Screen (placeholder)")
            Button(onClick = onBack) { Text("End Ride") }
        }
    }
}
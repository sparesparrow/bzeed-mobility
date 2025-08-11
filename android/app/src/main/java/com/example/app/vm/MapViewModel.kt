package com.example.app.vm

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.example.app.ble.BleService
import com.example.app.data.Repository
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.launch

@HiltViewModel
class MapViewModel @Inject constructor(
    private val repository: Repository,
    private val bleService: BleService
) : ViewModel() {
    private val _bluetoothEnabled = MutableStateFlow(bleService.isBluetoothEnabled())
    val bluetoothEnabled: StateFlow<Boolean> = _bluetoothEnabled

    private val _health = MutableStateFlow<String?>(null)
    val health: StateFlow<String?> = _health

    fun refreshHealth() {
        viewModelScope.launch {
            runCatching { repository.checkHealth() }
                .onSuccess { _health.value = it }
                .onFailure { _health.value = "error" }
        }
    }
}
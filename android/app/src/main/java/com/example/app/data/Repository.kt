package com.example.app.data

import javax.inject.Inject
import javax.inject.Singleton

@Singleton
class Repository @Inject constructor(private val api: ApiService) {
    suspend fun checkHealth(): String = api.health()
}
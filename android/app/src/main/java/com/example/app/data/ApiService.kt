package com.example.app.data

import retrofit2.http.GET

interface ApiService {
    @GET("health")
    suspend fun health(): String
}
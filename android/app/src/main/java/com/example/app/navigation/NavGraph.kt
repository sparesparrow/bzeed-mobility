package com.example.app.navigation

import androidx.compose.runtime.Composable
import androidx.hilt.navigation.compose.hiltViewModel
import androidx.navigation.NavHostController
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import com.example.app.ui.map.MapScreen
import com.example.app.ui.ride.RideScreen
import com.example.app.ui.profile.ProfileScreen
import com.example.app.vm.MapViewModel
import com.example.app.vm.RideViewModel
import com.example.app.vm.ProfileViewModel

sealed class Destinations(val route: String) {
    data object Map : Destinations("map")
    data object Ride : Destinations("ride")
    data object Profile : Destinations("profile")
}

@Composable
fun AppNavGraph(navController: NavHostController) {
    NavHost(navController = navController, startDestination = Destinations.Map.route) {
        composable(Destinations.Map.route) {
            val vm: MapViewModel = hiltViewModel()
            MapScreen(onStartRide = { navController.navigate(Destinations.Ride.route) }, onProfile = { navController.navigate(Destinations.Profile.route) }, viewModel = vm)
        }
        composable(Destinations.Ride.route) {
            val vm: RideViewModel = hiltViewModel()
            RideScreen(onBack = { navController.popBackStack() }, viewModel = vm)
        }
        composable(Destinations.Profile.route) {
            val vm: ProfileViewModel = hiltViewModel()
            ProfileScreen(onBack = { navController.popBackStack() }, viewModel = vm)
        }
    }
}
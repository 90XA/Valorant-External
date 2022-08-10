#pragma once
namespace VALROM {
	inline std::vector<Enemy> retreiveValidEnemies(uintptr_t actor_array, int actor_count) {
		std::vector<Enemy> temp_enemy_collection{};
		for (int i = 0; i < actor_count; i++) {
			uintptr_t actor = Read<uintptr_t>(actor_array + (i * 0x8));
			if (actor == NULL) {
				continue;
			}

			uintptr_t unique_id = Read<uintptr_t>(actor + offsets::unique_id);
			if (unique_id != 0x11e0101) {
				continue;
			}

			uintptr_t mesh = Read<uintptr_t>(actor + offsets::mesh_component);
			if (!mesh) {
				continue;
			}

			uintptr_t player_state = Read<uintptr_t>(actor + offsets::player_state);
			uintptr_t team_component = Read<uintptr_t>(player_state + offsets::team_component);
			int team_id = Read<int>(team_component + offsets::team_id);
			int bone_count = Read<int>(mesh + offsets::bone_count);
			
			if (team_id == g_local_team_id) {
				continue;
			}

			uintptr_t damage_handler = Read<uintptr_t>(actor + offsets::damage_handler);
			uintptr_t root_component = Read<uintptr_t>(actor + offsets::root_component);
			uintptr_t bone_array = Read<uintptr_t>(mesh + offsets::bone_array);
			uint32_t bone_array_count = Read<uint32_t>(mesh + offsets::bone_array + 8);
			if (!bone_array_count)
				bone_array = Read<uintptr_t>(mesh + offsets::bone_array);

			Enemy enemy{
				actor,
				damage_handler,
				player_state,
				root_component,
				mesh,
				bone_array,
				bone_count,
				true
			};
			temp_enemy_collection.push_back(enemy);
		}

		return temp_enemy_collection;
	}
	inline void retreiveData() {
		while (true) {
			if (!IsWindow(valorant_window))
				exit(-1);

			uintptr_t world = decryptWorld(g_base_address);
			auto persistent_level = Read<uintptr_t>(world + offsets::persistent_level);
			auto game_instance = Read<uintptr_t>(world + offsets::game_instance);
			auto local_player_array = Read<uintptr_t>(game_instance + offsets::local_player_array);
			auto local_player = Read<uintptr_t>(local_player_array);
			auto local_player_controller = Read<uintptr_t>(local_player + offsets::local_player_controller);
			auto local_player_pawn = Read<uintptr_t>(local_player_controller + offsets::local_player_pawn);
			auto local_damage_handler = Read<uintptr_t>(local_player_pawn + offsets::damage_handler);
			auto local_player_state = Read<uintptr_t>(local_player_pawn + offsets::player_state);
			auto local_team_component = Read<uintptr_t>(local_player_state + offsets::team_component);
			auto local_team_id = Read<int>(local_team_component + offsets::team_id);
			auto camera_manager = Read<uintptr_t>(local_player_controller + offsets::camera_manager);

			auto actor_array = Read<uintptr_t>(persistent_level + offsets::actor_array);
			auto actor_count = Read<int>(persistent_level + offsets::actor_count);
			g_local_player_controller = local_player_controller;
			g_local_player_pawn = local_player_pawn;
			g_local_damage_handler = local_damage_handler;
			g_camera_manager = camera_manager;
			g_local_team_id = local_team_id;

			enemy_collection = retreiveValidEnemies(actor_array, actor_count);
			Sleep(1500);
		}
	}

	inline Vector3 getBonePosition(Enemy enemy, int index) {

		FTransform Transform = Read<FTransform>(enemy.bone_array_ptr + (0x30 * index));
		FMatrix Matrix = Transform.ToMatrixWithScale() * Read<FTransform>(enemy.mesh_ptr + offsets::component_to_world).ToMatrixWithScale();
		return Vector3(Matrix.WPlane.X, Matrix.WPlane.Y, Matrix.WPlane.Z);
	}
	inline void renderBoneLine(Vector3 first_bone_position, Vector3 second_bone_position, Vector3 position, Vector3 rotation, float fov) {
		Vector2 first_bone_screen_position = worldToScreen(first_bone_position, position, rotation, fov);
		ImVec2 fist_screen_position = ImVec2(first_bone_screen_position.x, first_bone_screen_position.y);
		Vector2 second_bone_screen_position = worldToScreen(second_bone_position, position, rotation, fov);
		ImVec2 second_screen_position = ImVec2(second_bone_screen_position.x, second_bone_screen_position.y);
		ImGui::GetOverlayDrawList()->AddLine(fist_screen_position, second_screen_position, g_Config->g_color_white);
	}
	inline void renderBones(Enemy enemy, Vector3 position, Vector3 rotation, float fov) {
		Vector3 head_position = getBonePosition(enemy, 8);
		Vector3 neck_position;
		Vector3 chest_position = getBonePosition(enemy, 6);
		Vector3 l_upper_arm_position;
		Vector3 l_fore_arm_position;
		Vector3 l_hand_position;
		Vector3 r_upper_arm_position;
		Vector3 r_fore_arm_position;
		Vector3 r_hand_position;
		Vector3 stomach_position = getBonePosition(enemy, 4);
		Vector3 pelvis_position = getBonePosition(enemy, 3);
		Vector3 l_thigh_position;
		Vector3 l_knee_position;
		Vector3 l_foot_position;
		Vector3 r_thigh_position;
		Vector3 r_knee_position;
		Vector3 r_foot_position;
		if (enemy.bone_count == 102) { // MALE & FEMALE
			neck_position = getBonePosition(enemy, 19);

			l_upper_arm_position = getBonePosition(enemy, 21);
			l_fore_arm_position = getBonePosition(enemy, 22);
			l_hand_position = getBonePosition(enemy, 23);

			r_upper_arm_position = getBonePosition(enemy, 47);
			r_fore_arm_position = getBonePosition(enemy, 48);
			r_hand_position = getBonePosition(enemy, 49);

			l_thigh_position = getBonePosition(enemy, 75);
			l_knee_position = getBonePosition(enemy, 76);
			l_foot_position = getBonePosition(enemy, 78);

			r_thigh_position = getBonePosition(enemy, 82);
			r_knee_position = getBonePosition(enemy, 83);
			r_foot_position = getBonePosition(enemy, 85);
		}
		else if (enemy.bone_count == 101) {

			neck_position = getBonePosition(enemy, 21);

			l_upper_arm_position = getBonePosition(enemy, 45);
			l_fore_arm_position = getBonePosition(enemy, 42);
			l_hand_position = getBonePosition(enemy, 44);

			r_upper_arm_position = getBonePosition(enemy, 48);
			r_fore_arm_position = getBonePosition(enemy, 67);
			r_hand_position = getBonePosition(enemy, 69);

			l_thigh_position = getBonePosition(enemy, 75);
			l_knee_position = getBonePosition(enemy, 76);
			l_foot_position = getBonePosition(enemy, 78);

			r_thigh_position = getBonePosition(enemy, 82);
			r_knee_position = getBonePosition(enemy, 84);
			r_foot_position = getBonePosition(enemy, 85);

		}
		else if (enemy.bone_count == 103) { // BOT
			neck_position = getBonePosition(enemy, 9);

			l_upper_arm_position = getBonePosition(enemy, 33);
			l_fore_arm_position = getBonePosition(enemy, 30);
			l_hand_position = getBonePosition(enemy, 32);

			r_upper_arm_position = getBonePosition(enemy, 58);
			r_fore_arm_position = getBonePosition(enemy, 55);
			r_hand_position = getBonePosition(enemy, 57);

			l_thigh_position = getBonePosition(enemy, 63);
			l_knee_position = getBonePosition(enemy, 65);
			l_foot_position = getBonePosition(enemy, 69);

			r_thigh_position = getBonePosition(enemy, 77);
			r_knee_position = getBonePosition(enemy, 79);
			r_foot_position = getBonePosition(enemy, 83);
		}
		else {
			return;
		}

		renderBoneLine(head_position, neck_position, position, rotation, fov);
		renderBoneLine(neck_position, chest_position, position, rotation, fov);
		renderBoneLine(neck_position, l_upper_arm_position, position, rotation, fov);
		renderBoneLine(l_upper_arm_position, l_fore_arm_position, position, rotation, fov);
		renderBoneLine(l_fore_arm_position, l_hand_position, position, rotation, fov);
		renderBoneLine(neck_position, r_upper_arm_position, position, rotation, fov);
		renderBoneLine(r_upper_arm_position, r_fore_arm_position, position, rotation, fov);
		renderBoneLine(r_fore_arm_position, r_hand_position, position, rotation, fov);
		renderBoneLine(chest_position, stomach_position, position, rotation, fov);
		renderBoneLine(stomach_position, pelvis_position, position, rotation, fov);
		renderBoneLine(pelvis_position, l_thigh_position, position, rotation, fov);
		renderBoneLine(l_thigh_position, l_knee_position, position, rotation, fov);
		renderBoneLine(l_knee_position, l_foot_position, position, rotation, fov);
		renderBoneLine(pelvis_position, r_thigh_position, position, rotation, fov);
		renderBoneLine(r_thigh_position, r_knee_position, position, rotation, fov);
		renderBoneLine(r_knee_position, r_foot_position, position, rotation, fov);
	}
	inline void renderBox(Vector2 head_at_screen, float distance_modifier) {
		int head_x = head_at_screen.x;
		int head_y = head_at_screen.y;
		int start_x = head_x - 35 / distance_modifier;
		int start_y = head_y - 15 / distance_modifier;
		int end_x = head_x + 35 / distance_modifier;
		int end_y = head_y + 155 / distance_modifier;
		ImGui::GetOverlayDrawList()->AddRect(ImVec2(start_x, start_y), ImVec2(end_x, end_y), g_Config->g_esp_color);
	}

	inline Vector3 SmoothAim(Vector3 CameraRotation, Vector3 Target, float SmoothFactor)
	{
		Vector3 delta = (Target - CameraRotation).Clamp();

		float smooth = powf(SmoothFactor, 0.4f);
		smooth = min(0.99f, smooth);

		float coeff = (1.0f - smooth) / delta.Length2D() * 0.5f;
		coeff = min(0.99f, coeff);

		return delta * coeff;
	}

	inline void Aimbot()
	{
		for (;;)
		{
			if (g_Config->g_Aimbot_enable) {
				while (GetAsyncKeyState(VK_XBUTTON2))
				{
					std::vector<Target> possible;

					Vector3 Location = Read<Vector3>(g_camera_manager + offsets::camera_position);
					Vector3 Rocation = Read<Vector3>(g_camera_manager + offsets::camera_rotation);
					float Fov = Read<float>(g_camera_manager + offsets::camera_fov);

					for (int i = 0; i < enemy_collection.size(); ++i)
					{
						Enemy enemy = enemy_collection[i];
						float Health = Read<float>(enemy.damage_handler_ptr + offsets::health);
						if (enemy.actor_ptr == g_local_player_pawn || Health <= 0 || !enemy.mesh_ptr) {
							continue;
						}

						uintptr_t team_component = Read<uintptr_t>(enemy.player_state_ptr + offsets::team_component);
						int team_id = Read<int>(team_component + offsets::team_id);

						if (team_id == g_local_team_id) {
							continue;
						}

						float last_render_time = Read<float>(enemy.mesh_ptr + offsets::last_render_time);
						float last_submit_time = Read<float>(enemy.mesh_ptr + offsets::last_submit_time);
						bool is_visible = last_render_time + 0.06F >= last_submit_time;
						bool dormant = Read<bool>(enemy.actor_ptr + offsets::dormant);
						if (!dormant || !is_visible) {
							continue;
						}

						Vector3 head_position = getBonePosition(enemy, 8); // 8 = head bone
						Vector2 outPos = worldToScreen(head_position, Location, Rocation, Fov);
						Vector3 mPos = { outPos.x - GetSystemMetrics(SM_CXSCREEN) / 2, outPos.y - GetSystemMetrics(SM_CYSCREEN) / 2, 0 };

						float fovDist = mPos.Length();
						if (fovDist <= g_Config->g_AimFov) {
							possible.emplace_back(enemy, head_position, fovDist);
						}
					}

					if (possible.size())
					{
						std::sort(possible.begin(), possible.end(), AimSortCrossHair);

						Target currentTarget = possible[0];

						Vector3 TargetPos = currentTarget.headPos;
						Vector3 TargetRotation = (Location - TargetPos).ToRotator().Clamp();

						Vector3 ConvertRotation = Rocation.Clamp();

						Vector3 ControlRotation = Read<Vector3>(g_local_player_controller + offsets::control_rotation);
						Vector3 DeltaRotation = (ConvertRotation - ControlRotation).Clamp();

						DeltaRotation.y /= 4.0f;
						ConvertRotation = (TargetRotation - (DeltaRotation)).Clamp();

						Vector3 Smoothed = SmoothAim(Rocation, ConvertRotation, g_Config->g_AimSmooth);

						{
							READ_GUARDED_REGION_REQUEST Request = {};
							DWORD cbReturned = 0;
							BOOL status = TRUE;

							ULONGLONG Displacement = (g_local_player_controller + 0x05E4 - 0xC) & 0xFFFFFF;

							if (Displacement < 0x200000)
							{
								uint64_t tmp;

								Request.Displacement = Displacement;
								Request.Buffer = &tmp;
								Request.Size = 8;
								Request.X = Smoothed.x;
								Request.Y = Smoothed.y;

								status = DeviceIoControl(
									mem->hDevice,
									IOCTL_READ_GUARDED_REGION,
									&Request,
									sizeof(Request),
									&Request,
									sizeof(Request),
									&cbReturned,
									NULL);
							}
						}
					}

					Sleep(1);
				}
			}
		}
	}
	inline void renderEsp() {
		std::vector<Enemy> local_enemy_collection = enemy_collection;
		if (local_enemy_collection.empty()) {
			return;
		}

		Vector3 camera_position = Read<Vector3>(g_camera_manager + offsets::camera_position);
		Vector3 camera_rotation = Read<Vector3>(g_camera_manager + offsets::camera_rotation);
		float camera_fov = Read<float>(g_camera_manager + offsets::camera_fov);

		for (int i = 0; i < local_enemy_collection.size(); i++) {
			Enemy enemy = local_enemy_collection[i];
			float health = Read<float>(enemy.damage_handler_ptr + offsets::health);
			if (enemy.actor_ptr == g_local_player_pawn || health <= 0 || !enemy.mesh_ptr) {
				continue;
			}
			/*const TArray<FTransform> TransformList = Read<TArray<FTransform>>(enemy.mesh_ptr + offsets::bone_array);

			if (TransformList.IsValid() && TransformList.Num() > 90 && TransformList.Num() < 110)
			{
				std::vector<FTransform> vTransformList = TransformList.GetVector();

				FTransform ComponentToWorld = Read<FTransform>(enemy.mesh_ptr + offsets::component_to_world);

				for (size_t x = 0; x < vTransformList.size(); ++x)
				{
					FMatrix Matrix = vTransformList[x].ToMatrixWithScale() * ComponentToWorld.ToMatrixWithScale();
					Vector3 CurrentBone = Vector3{ Matrix.WPlane.X, Matrix.WPlane.Y, Matrix.WPlane.Z };

					{
						Vector2 v2_Location = worldToScreen(CurrentBone, camera_position, camera_rotation, camera_fov);
						ImGui::GetOverlayDrawList()->AddCircle({ v2_Location.x, v2_Location.y }, 5, g_Config->g_esp_color, 0, 3);
					}
				}
			}*/
			Vector3 head_position = getBonePosition(enemy, 8); // 8 = head bone
			Vector3 root_position = getBonePosition(enemy, 0); // 8 = head bone

			if (head_position.z <= root_position.z) {
				continue;
			}

			Vector2 head_at_screen_vec = worldToScreen(head_position, camera_position, camera_rotation, camera_fov);
			ImVec2 head_at_screen = ImVec2(head_at_screen_vec.x, head_at_screen_vec.y);
			float distance_modifier = camera_position.Distance(head_position) * 0.001F;
			if (g_Config->g_boneesp) {
				renderBones(enemy, camera_position, camera_rotation, camera_fov);
			}
			if (g_Config->g_headesp) {
				ImGui::GetOverlayDrawList()->AddCircle(head_at_screen, 7 / distance_modifier, g_Config->g_esp_color, 0, 3);
			}
			if (g_Config->g_boxesp) {
				renderBox(head_at_screen_vec, distance_modifier);
			}

			if (g_Config->g_AmmoCount) {


				auto Inventory = Read<uintptr_t>(g_pid, enemy.actor_ptr + offsets::inventory);
				intptr_t CurrentEquip = Read<uintptr_t>(g_pid, Inventory + offsets::current_equipable);
				uintptr_t ammo = Read<uintptr_t>(g_pid, CurrentEquip + offsets::ammo_info);
				int maxammo = Read<int>(g_pid, ammo + offsets::max_ammo);
				int currentammo = Read<int>(g_pid, ammo + offsets::current_ammo);
				char text[64];
				sprintf_s(text, ("%d"), maxammo);
				char text2[64];
				sprintf_s(text2, ("%d"), currentammo);
				std::string MaxAmmo = text;
				std::string CurrentAmmo = text2;
				std::string slash = "/";
				std::string aimbot = "Ammo ";
				std::string AmmoInfo = aimbot + text2 + slash + text;
				DrawString(14, leg_vec.x, leg_vec.y + 35, &Col.green, true, true, AmmoInfo.c_str());
			}
		}
	}
	inline void runRenderTick() {
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (g_Config->g_esp_enabled) {
			renderEsp();
			ImGui::GetOverlayDrawList()->AddCircle(ImVec2(GetSystemMetrics(SM_CXSCREEN) / 2.0f, GetSystemMetrics(SM_CYSCREEN) / 2.0f), g_Config->g_AimFov, g_Config->g_color_white, 0, 2);
		}

		if (g_Config->g_overlay_visible) {
			// Visuals Window
			{
				ImGui::SetNextWindowPos(ImVec2(1520.f, 0.f), ImGuiCond_Once);
				ImGui::SetNextWindowSize(ImVec2(400.f, 1100.f), ImGuiCond_Once);

				ImGui::Begin(skCrypt(u8"VALROM"), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);

				ImGui::BulletText(skCrypt(u8"Visuals"));

				ImGui::Checkbox(skCrypt(u8"ESP Enable"), &g_Config->g_esp_enabled);
				ImGui::Checkbox(skCrypt(u8"ESP Vis Check"), &g_Config->g_esp_dormantcheck);
				ImGui::Checkbox(skCrypt(u8"Head ESP"), &g_Config->g_headesp);
				ImGui::Checkbox(skCrypt(u8"Bone ESP"), &g_Config->g_boneesp);
				ImGui::Checkbox(skCrypt(u8"Box ESP"), &g_Config->g_boxesp);
				ImGui::Separator();

				ImGui::BulletText(skCrypt(u8"Aimbot"));

				ImGui::Checkbox(skCrypt("Aimbot Enable"), &g_Config->g_Aimbot_enable);
				if (g_Config->g_Aimbot_enable) {
					ImGui::Checkbox(skCrypt("Fov Show"), &g_Config->g_DrawFov);
					ImGui::SliderFloat(skCrypt("Smoothed"), &g_Config->g_AimSmooth, 0.0f, 1.f, skCrypt("Smooth %.2f"));
					ImGui::SliderFloat(skCrypt("Fov"), &g_Config->g_AimFov, 0.0f, 300.f, skCrypt("FOV %.2f"));
				}
				ImGui::Separator();
				ImGui::End();
			}
		}
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(g_window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(g_window);
	}
}
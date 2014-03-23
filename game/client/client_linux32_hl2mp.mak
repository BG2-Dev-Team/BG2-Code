NAME=client_hl2mp
SRCROOT=../..
TARGET_PLATFORM=linux32
TARGET_PLATFORM_EXT=
USE_VALVE_BINDIR=0
PWD:=$(shell pwd)
# If no configuration is specified, "release" will be used.
ifeq "$(CFG)" ""
	CFG = release
endif

GCC_ExtraCompilerFlags=
GCC_ExtraLinkerFlags=
SymbolVisibility=hidden
#OptimizerLevel=-gdwarf-2 -g2 $(OptimizerLevel_CompilerSpecific)
OptimizerLevel=-O2
SystemLibraries=-lrt 
DLL_EXT=.so
SYM_EXT=.dbg
FORCEINCLUDES= 
DEFINES += -DVPC -DRAD_TELEMETRY_DISABLED -DNDEBUG -DGNUC -DPOSIX -DCOMPILER_GCC -D_DLL_EXT=.so -D_LINUX -DLINUX -DPOSIX -D_POSIX -DDLLNAME=client -DBINK_VIDEO -DGL_GLEXT_PROTOTYPES -DDX_TO_GL_ABSTRACTION -DUSE_SDL -DDEV_BUILD -DNO_STRING_T -DCLIENT_DLL -DVECTOR -DVERSION_SAFE_STEAM_API_INTERFACES -DPROTECTED_THINGS_ENABLE -Dstrncpy=use_Q_strncpy_instead -D_snprintf=use_Q_snprintf_instead -DENABLE_CHROMEHTMLWINDOW -DUSE_WEBM_FOR_REPLAY -DHL2MP -DHL2_CLIENT_DLL -D_EXTERNAL_DLL_EXT=.so -DVPCGAMECAPS=HL2MP -DPROJECTDIR=/root/source2/game/client -D_DLL_EXT=.so -DSOURCE1=1 -DVPCGAME=hl2mp -D_LINUX=1 -D_POSIX=1 -DLINUX=1 -DPOSIX=1 
INCLUDEDIRS += ./ ../../common ../../public ../../public/tier0 ../../public/tier1 ../../thirdparty/SDL2 ../../game/client/generated_proto_mod_hl2mp ../../thirdparty/protobuf-2.3.0/src ../../vgui2/include ../../vgui2/controls ../../game/shared ./game_controls ../../thirdparty/sixensesdk/include hl2mp/ui ./hl2mp ../../game/shared/hl2mp ./hl2 ./hl2/elements ../../game/shared/hl2 
CONFTYPE=dll
IMPORTLIBRARY=
GAMEOUTPUTFILE=../../../srcds/2013/bg2-svn/bin/client.so
OUTPUTFILE=$(OBJ_DIR)/client.so


POSTBUILDCOMMAND=true



CPPFILES= \
    ../../common/compiledcaptionswap.cpp \
    ../../common/language.cpp \
    ../../common/randoverride.cpp \
    ../../game/client/c_vote_controller.cpp \
    ../../game/shared/achievementmgr.cpp \
    ../../game/shared/achievements_hlx.cpp \
    ../../game/shared/achievement_saverestore.cpp \
    ../../game/shared/activitylist.cpp \
    ../../game/shared/ammodef.cpp \
    ../../game/shared/animation.cpp \
    ../../game/shared/baseachievement.cpp \
    ../../game/shared/basecombatcharacter_shared.cpp \
    ../../game/shared/basecombatweapon_shared.cpp \
    ../../game/shared/baseentity_shared.cpp \
    ../../game/shared/basegrenade_shared.cpp \
    ../../game/shared/baseparticleentity.cpp \
    ../../game/shared/baseplayer_shared.cpp \
    ../../game/shared/baseprojectile.cpp \
    ../../game/shared/baseviewmodel_shared.cpp \
    ../../game/shared/base_playeranimstate.cpp \
    ../../game/shared/beam_shared.cpp \
    ../../game/shared/cam_thirdperson.cpp \
    ../../game/shared/collisionproperty.cpp \
    ../../game/shared/death_pose.cpp \
    ../../game/shared/debugoverlay_shared.cpp \
    ../../game/shared/decals.cpp \
    ../../game/shared/effect_dispatch_data.cpp \
    ../../game/shared/ehandle.cpp \
    ../../game/shared/entitylist_base.cpp \
    ../../game/shared/EntityParticleTrail_Shared.cpp \
    ../../game/shared/env_detail_controller.cpp \
    ../../game/shared/env_wind_shared.cpp \
    ../../game/shared/eventlist.cpp \
    ../../game/shared/func_ladder.cpp \
    ../../game/shared/gamemovement.cpp \
    ../../game/shared/gamerules.cpp \
    ../../game/shared/gamerules_register.cpp \
    ../../game/shared/GameStats.cpp \
    ../../game/shared/gamestringpool.cpp \
    ../../game/shared/gamevars_shared.cpp \
    ../../game/shared/hintmessage.cpp \
    ../../game/shared/hintsystem.cpp \
    ../../game/shared/hl2/basehlcombatweapon_shared.cpp \
    ../../game/shared/hl2/env_headcrabcanister_shared.cpp \
    ../../game/shared/hl2/hl2_gamerules.cpp \
    ../../game/shared/hl2/hl2_usermessages.cpp \
    ../../game/shared/hl2/hl_gamemovement.cpp \
    ../../game/shared/hl2mp/hl2mp_gamerules.cpp \
    ../../game/shared/hl2mp/hl2mp_player_shared.cpp \
    ../../game/shared/hl2mp/hl2mp_weapon_parse.cpp \
    ../../game/shared/hl2mp/weapon_357.cpp \
    ../../game/shared/hl2mp/weapon_ar2.cpp \
    ../../game/shared/hl2mp/weapon_crossbow.cpp \
    ../../game/shared/hl2mp/weapon_crowbar.cpp \
    ../../game/shared/hl2mp/weapon_frag.cpp \
    ../../game/shared/hl2mp/weapon_hl2mpbase.cpp \
    ../../game/shared/hl2mp/weapon_hl2mpbasebasebludgeon.cpp \
    ../../game/shared/hl2mp/weapon_hl2mpbasehlmpcombatweapon.cpp \
    ../../game/shared/hl2mp/weapon_hl2mpbase_machinegun.cpp \
    ../../game/shared/hl2mp/weapon_physcannon.cpp \
    ../../game/shared/hl2mp/weapon_pistol.cpp \
    ../../game/shared/hl2mp/weapon_rpg.cpp \
    ../../game/shared/hl2mp/weapon_shotgun.cpp \
    ../../game/shared/hl2mp/weapon_slam.cpp \
    ../../game/shared/hl2mp/weapon_smg1.cpp \
    ../../game/shared/hl2mp/weapon_stunstick.cpp \
    ../../game/shared/igamesystem.cpp \
    ../../game/shared/interval.cpp \
    ../../game/shared/mapentities_shared.cpp \
    ../../game/shared/movevars_shared.cpp \
    ../../game/shared/mp_shareddefs.cpp \
    ../../game/shared/multiplay_gamerules.cpp \
    ../../game/shared/obstacle_pushaway.cpp \
    ../../game/shared/particlesystemquery.cpp \
    ../../game/shared/particle_parse.cpp \
    ../../game/shared/particle_property.cpp \
    ../../game/shared/physics_main_shared.cpp \
    ../../game/shared/physics_saverestore.cpp \
    ../../game/shared/physics_shared.cpp \
    ../../game/shared/point_bonusmaps_accessor.cpp \
    ../../game/shared/point_posecontroller.cpp \
    ../../game/shared/precache_register.cpp \
    ../../game/shared/predictableid.cpp \
    ../../game/shared/predicted_viewmodel.cpp \
    ../../game/shared/predictioncopy.cpp \
    ../../game/shared/props_shared.cpp \
    ../../game/shared/ragdoll_shared.cpp \
    ../../game/shared/rope_helpers.cpp \
    ../../game/shared/saverestore.cpp \
    ../../game/shared/sceneentity_shared.cpp \
    ../../game/shared/script_intro_shared.cpp \
    ../../game/shared/sequence_Transitioner.cpp \
    ../../game/shared/sheetsimulator.cpp \
    ../../game/shared/simtimer.cpp \
    ../../game/shared/singleplay_gamerules.cpp \
    ../../game/shared/sixense/sixense_convars.cpp \
    ../../game/shared/SoundEmitterSystem.cpp \
    ../../game/shared/soundenvelope.cpp \
    ../../game/shared/Sprite.cpp \
    ../../game/shared/SpriteTrail.cpp \
    ../../game/shared/studio_shared.cpp \
    ../../game/shared/takedamageinfo.cpp \
    ../../game/shared/teamplayroundbased_gamerules.cpp \
    ../../game/shared/teamplay_gamerules.cpp \
    ../../game/shared/teamplay_round_timer.cpp \
    ../../game/shared/test_ehandle.cpp \
    ../../game/shared/usercmd.cpp \
    ../../game/shared/usermessages.cpp \
    ../../game/shared/util_shared.cpp \
    ../../game/shared/vehicle_viewblend_shared.cpp \
    ../../game/shared/voice_banmgr.cpp \
    ../../game/shared/voice_status.cpp \
    ../../game/shared/weapon_parse.cpp \
    ../../public/bone_accessor.cpp \
    ../../public/bone_setup.cpp \
    ../../public/client_class.cpp \
    ../../public/collisionutils.cpp \
    ../../public/crtmemdebug.cpp \
    ../../public/dt_recv.cpp \
    ../../public/dt_utlvector_common.cpp \
    ../../public/dt_utlvector_recv.cpp \
    ../../public/filesystem_helpers.cpp \
    ../../public/haptics/haptic_msgs.cpp \
    ../../public/interpolatortypes.cpp \
    ../../public/jigglebones.cpp \
    ../../public/networkvar.cpp \
    ../../public/posedebugger.cpp \
    ../../public/renamed_recvtable_compat.cpp \
    ../../public/rope_physics.cpp \
    ../../public/scratchpad3d.cpp \
    ../../public/ScratchPadUtils.cpp \
    ../../public/sentence.cpp \
    ../../public/simple_physics.cpp \
    ../../public/SoundParametersInternal.cpp \
    ../../public/stringregistry.cpp \
    ../../public/studio.cpp \
    ../../public/tier0/memoverride.cpp \
    ../../public/tools/bonelist.cpp \
    ../../public/vallocator.cpp \
    ../../public/vgui_controls/vgui_controls.cpp \
    achievement_notification_panel.cpp \
    alphamaterialproxy.cpp \
    animatedentitytextureproxy.cpp \
    animatedoffsettextureproxy.cpp \
    animatedtextureproxy.cpp \
    AnimateSpecificTextureProxy.cpp \
    baseanimatedtextureproxy.cpp \
    baseclientrendertargets.cpp \
    basepresence.cpp \
    beamdraw.cpp \
    bone_merge_cache.cpp \
    camomaterialproxy.cpp \
    cdll_bounded_cvars.cpp \
    cdll_client_int.cpp \
    cdll_util.cpp \
    classmap.cpp \
    clienteffectprecachesystem.cpp \
    cliententitylist.cpp \
    clientleafsystem.cpp \
    clientmode_shared.cpp \
    clientshadowmgr.cpp \
    clientsideeffects.cpp \
    clientsideeffects_test.cpp \
    clientsteamcontext.cpp \
    client_factorylist.cpp \
    client_thinklist.cpp \
    client_virtualreality.cpp \
    cl_mat_stub.cpp \
    colorcorrectionmgr.cpp \
    commentary_modelviewer.cpp \
    c_ai_basehumanoid.cpp \
    c_ai_basenpc.cpp \
    c_baseanimating.cpp \
    c_baseanimatingoverlay.cpp \
    c_basecombatcharacter.cpp \
    c_basecombatweapon.cpp \
    c_basedoor.cpp \
    c_baseentity.cpp \
    c_baseflex.cpp \
    c_baseplayer.cpp \
    c_basetempentity.cpp \
    c_baseviewmodel.cpp \
    c_breakableprop.cpp \
    c_colorcorrection.cpp \
    c_colorcorrectionvolume.cpp \
    c_dynamiclight.cpp \
    c_effects.cpp \
    c_entitydissolve.cpp \
    c_entityparticletrail.cpp \
    c_env_fog_controller.cpp \
    c_env_particlescript.cpp \
    c_env_projectedtexture.cpp \
    c_env_screenoverlay.cpp \
    c_env_tonemap_controller.cpp \
    c_fire_smoke.cpp \
    c_fish.cpp \
    c_func_areaportalwindow.cpp \
    c_func_breakablesurf.cpp \
    c_func_conveyor.cpp \
    c_func_dust.cpp \
    c_func_lod.cpp \
    c_func_occluder.cpp \
    c_func_reflective_glass.cpp \
    c_func_rotating.cpp \
    c_func_smokevolume.cpp \
    c_func_tracktrain.cpp \
    c_gib.cpp \
    c_hairball.cpp \
    c_impact_effects.cpp \
    c_info_overlay_accessor.cpp \
    c_lightglow.cpp \
    C_MaterialModifyControl.cpp \
    c_movie_explosion.cpp \
    c_particle_fire.cpp \
    c_particle_smokegrenade.cpp \
    c_particle_system.cpp \
    c_physbox.cpp \
    c_physicsprop.cpp \
    c_physmagnet.cpp \
    c_pixel_visibility.cpp \
    c_plasma.cpp \
    c_playerresource.cpp \
    c_point_camera.cpp \
    c_point_commentary_node.cpp \
    c_props.cpp \
    c_prop_vehicle.cpp \
    c_ragdoll_manager.cpp \
    c_recipientfilter.cpp \
    c_rope.cpp \
    c_rumble.cpp \
    c_sceneentity.cpp \
    c_shadowcontrol.cpp \
    c_slideshow_display.cpp \
    c_smokestack.cpp \
    c_smoke_trail.cpp \
    c_soundscape.cpp \
    c_spotlight_end.cpp \
    c_sprite.cpp \
    c_sprite_perfmonitor.cpp \
    c_steamjet.cpp \
    c_stickybolt.cpp \
    c_sun.cpp \
    c_te.cpp \
    c_team.cpp \
    c_team_objectiveresource.cpp \
    c_team_train_watcher.cpp \
    c_tesla.cpp \
    c_testtraceline.cpp \
    c_test_proxytoggle.cpp \
    c_te_armorricochet.cpp \
    c_te_basebeam.cpp \
    c_te_beamentpoint.cpp \
    c_te_beaments.cpp \
    c_te_beamfollow.cpp \
    c_te_beamlaser.cpp \
    c_te_beampoints.cpp \
    c_te_beamring.cpp \
    c_te_beamringpoint.cpp \
    c_te_beamspline.cpp \
    c_te_bloodsprite.cpp \
    c_te_bloodstream.cpp \
    c_te_breakmodel.cpp \
    c_te_bspdecal.cpp \
    c_te_bubbles.cpp \
    c_te_bubbletrail.cpp \
    c_te_clientprojectile.cpp \
    c_te_decal.cpp \
    c_te_dynamiclight.cpp \
    c_te_effect_dispatch.cpp \
    c_te_energysplash.cpp \
    c_te_explosion.cpp \
    c_te_fizz.cpp \
    c_te_footprint.cpp \
    c_te_glassshatter.cpp \
    c_te_glowsprite.cpp \
    c_te_impact.cpp \
    c_te_killplayerattachments.cpp \
    c_te_largefunnel.cpp \
    c_te_legacytempents.cpp \
    c_te_muzzleflash.cpp \
    c_te_particlesystem.cpp \
    c_te_physicsprop.cpp \
    c_te_playerdecal.cpp \
    c_te_projecteddecal.cpp \
    c_te_showline.cpp \
    c_te_smoke.cpp \
    c_te_sparks.cpp \
    c_te_sprite.cpp \
    c_te_spritespray.cpp \
    c_te_worlddecal.cpp \
    c_tracer.cpp \
    c_user_message_register.cpp \
    c_vehicle_choreo_generic.cpp \
    c_vehicle_jeep.cpp \
    c_vguiscreen.cpp \
    C_WaterLODControl.cpp \
    c_world.cpp \
    detailobjectsystem.cpp \
    dummyproxy.cpp \
    EffectsClient.cpp \
    entityoriginmaterialproxy.cpp \
    entity_client_tools.cpp \
    episodic/c_vort_charge_token.cpp \
    flashlighteffect.cpp \
    functionproxy.cpp \
    fx.cpp \
    fx_blood.cpp \
    fx_cube.cpp \
    fx_discreetline.cpp \
    fx_envelope.cpp \
    fx_explosion.cpp \
    fx_fleck.cpp \
    fx_impact.cpp \
    fx_interpvalue.cpp \
    fx_line.cpp \
    fx_quad.cpp \
    fx_shelleject.cpp \
    fx_sparks.cpp \
    fx_staticline.cpp \
    fx_tracer.cpp \
    fx_trail.cpp \
    fx_water.cpp \
    gametrace_client.cpp \
    game_controls/basemodelpanel.cpp \
    game_controls/basemodel_panel.cpp \
    game_controls/baseviewport.cpp \
    game_controls/ClientScoreBoardDialog.cpp \
    game_controls/commandmenu.cpp \
    game_controls/IconPanel.cpp \
    game_controls/intromenu.cpp \
    game_controls/MapOverview.cpp \
    game_controls/NavProgress.cpp \
    game_controls/SpectatorGUI.cpp \
    game_controls/teammenu.cpp \
    game_controls/vguitextwindow.cpp \
    geiger.cpp \
    glow_outline_effect.cpp \
    glow_overlay.cpp \
    history_resource.cpp \
    hl2/c_antlion_dust.cpp \
    hl2/c_ar2_explosion.cpp \
    hl2/c_barnacle.cpp \
    hl2/c_barney.cpp \
    hl2/c_basehelicopter.cpp \
    hl2/c_basehlcombatweapon.cpp \
    hl2/c_basehlplayer.cpp \
    hl2/c_citadel_effects.cpp \
    hl2/c_corpse.cpp \
    hl2/c_env_alyxtemp.cpp \
    hl2/c_env_headcrabcanister.cpp \
    hl2/c_env_starfield.cpp \
    hl2/C_Func_Monitor.cpp \
    hl2/c_func_tankmortar.cpp \
    hl2/c_hl2_playerlocaldata.cpp \
    hl2/c_info_teleporter_countdown.cpp \
    hl2/c_npc_antlionguard.cpp \
    hl2/c_npc_combinegunship.cpp \
    hl2/c_npc_manhack.cpp \
    hl2/c_npc_rollermine.cpp \
    hl2/c_plasma_beam_node.cpp \
    hl2/c_prop_combine_ball.cpp \
    hl2/c_rotorwash.cpp \
    hl2/c_script_intro.cpp \
    hl2/c_strider.cpp \
    hl2/c_te_concussiveexplosion.cpp \
    hl2/c_te_flare.cpp \
    hl2/c_thumper_dust.cpp \
    hl2/c_vehicle_airboat.cpp \
    hl2/c_vehicle_cannon.cpp \
    hl2/c_vehicle_crane.cpp \
    hl2/c_vehicle_prisoner_pod.cpp \
    hl2/c_waterbullet.cpp \
    hl2/c_weapon_crossbow.cpp \
    hl2/c_weapon__stubs_hl2.cpp \
    hl2/fx_antlion.cpp \
    hl2/fx_bugbait.cpp \
    hl2/fx_hl2_impacts.cpp \
    hl2/fx_hl2_tracers.cpp \
    hl2/hl2_clientmode.cpp \
    hl2/hl_in_main.cpp \
    hl2/hl_prediction.cpp \
    hl2/hud_ammo.cpp \
    hl2/hud_autoaim.cpp \
    hl2/hud_battery.cpp \
    hl2/hud_blood.cpp \
    hl2/hud_credits.cpp \
    hl2/hud_damageindicator.cpp \
    hl2/hud_filmdemo.cpp \
    hl2/hud_flashlight.cpp \
    hl2/hud_hdrdemo.cpp \
    hl2/hud_health.cpp \
    hl2/hud_poisondamageindicator.cpp \
    hl2/hud_quickinfo.cpp \
    hl2/hud_suitpower.cpp \
    hl2/hud_weaponselection.cpp \
    hl2/hud_zoom.cpp \
    hl2/shieldproxy.cpp \
    hl2/vgui_rootpanel_hl2.cpp \
    hl2mp/clientmode_hl2mpnormal.cpp \
    hl2mp/c_hl2mp_player.cpp \
    hl2mp/c_te_hl2mp_shotgun_shot.cpp \
    hl2mp/hl2mp_hud_chat.cpp \
    hl2mp/hl2mp_hud_target_id.cpp \
    hl2mp/hl2mp_hud_team.cpp \
    hl2mp/hud_deathnotice.cpp \
    hl2mp/ui/backgroundpanel.cpp \
    hl2mp/ui/hl2mpclientscoreboard.cpp \
    hl2mp/ui/hl2mptextwindow.cpp \
    hltvcamera.cpp \
    hud.cpp \
    hud_animationinfo.cpp \
    hud_basechat.cpp \
    hud_basetimer.cpp \
    hud_bitmapnumericdisplay.cpp \
    hud_closecaption.cpp \
    hud_crosshair.cpp \
    hud_element_helper.cpp \
    hud_hintdisplay.cpp \
    hud_lcd.cpp \
    hud_msg.cpp \
    hud_numericdisplay.cpp \
    hud_pdump.cpp \
    hud_redraw.cpp \
    hud_squadstatus.cpp \
    hud_vehicle.cpp \
    hud_voicestatus.cpp \
    hud_weapon.cpp \
    initializer.cpp \
    interpolatedvar.cpp \
    in_camera.cpp \
    in_joystick.cpp \
    in_main.cpp \
    in_mouse.cpp \
    IsNPCProxy.cpp \
    lampbeamproxy.cpp \
    lamphaloproxy.cpp \
    mathproxy.cpp \
    matrixproxy.cpp \
    menu.cpp \
    message.cpp \
    movehelper_client.cpp \
    mp3player.cpp \
    mumble.cpp \
    panelmetaclassmgr.cpp \
    particlemgr.cpp \
    particlesphererenderer.cpp \
    particles_attractor.cpp \
    particles_ez.cpp \
    particles_localspace.cpp \
    particles_new.cpp \
    particles_simple.cpp \
    particle_collision.cpp \
    particle_litsmokeemitter.cpp \
    particle_proxies.cpp \
    particle_simple3d.cpp \
    perfvisualbenchmark.cpp \
    physics.cpp \
    physics_main_client.cpp \
    physpropclientside.cpp \
    playerandobjectenumerator.cpp \
    playerspawncache.cpp \
    prediction.cpp \
    proxyentity.cpp \
    ProxyHealth.cpp \
    proxyplayer.cpp \
    proxypupil.cpp \
    ragdoll.cpp \
    recvproxy.cpp \
    rendertexture.cpp \
    replay/cdll_replay.cpp \
    replay/replaycamera.cpp \
    ScreenSpaceEffects.cpp \
    simple_keys.cpp \
    sixense/in_sixense.cpp \
    sixense/in_sixense_gesture_bindings.cpp \
    smoke_fog_overlay.cpp \
    splinepatch.cpp \
    spritemodel.cpp \
    stdafx.cpp \
    studio_stats.cpp \
    texturescrollmaterialproxy.cpp \
    text_message.cpp \
    timematerialproxy.cpp \
    toggletextureproxy.cpp \
    toolframework_client.cpp \
    train.cpp \
    vgui_avatarimage.cpp \
    vgui_basepanel.cpp \
    vgui_bitmapbutton.cpp \
    vgui_bitmapimage.cpp \
    vgui_bitmappanel.cpp \
    vgui_centerstringpanel.cpp \
    vgui_consolepanel.cpp \
    vgui_debugoverlaypanel.cpp \
    vgui_fpspanel.cpp \
    vgui_game_viewport.cpp \
    vgui_grid.cpp \
    vgui_int.cpp \
    vgui_loadingdiscpanel.cpp \
    vgui_messagechars.cpp \
    vgui_netgraphpanel.cpp \
    vgui_schemevisualizer.cpp \
    vgui_slideshow_display_screen.cpp \
    vgui_video.cpp \
    vgui_video_player.cpp \
    view.cpp \
    viewangleanim.cpp \
    ViewConeImage.cpp \
    viewdebug.cpp \
    viewpostprocess.cpp \
    viewrender.cpp \
    view_beams.cpp \
    view_effects.cpp \
    view_scene.cpp \
    warp_overlay.cpp \
    WaterLODMaterialProxy.cpp \
    weapons_resource.cpp \
    weapon_selection.cpp \
    WorldDimsProxy.cpp \


LIBFILES = \
    ../../lib/public/linux32/tier1.a \
    ../../lib/public/linux32/libprotobuf.a \
    ../../lib/public/linux32/bitmap.a \
    ../../lib/public/linux32/choreoobjects.a \
    ../../lib/public/linux32/dmxloader.a \
    ../../lib/public/linux32/mathlib.a \
    ../../lib/public/linux32/matsys_controls.a \
    ../../lib/public/linux32/particles.a \
    ../../lib/public/linux32/tier1.a \
    ../../lib/public/linux32/tier2.a \
    ../../lib/public/linux32/tier3.a \
    ../../lib/public/linux32/vgui_controls.a \
    ../../lib/public/linux32/vtf.a \
    ../../lib/common/linux32/libcrypto.a \
    ../../lib/public/linux32/libz.a \
    ../../lib/common/linux32/libcurl.a \
    ../../lib/common/linux32/libcurlssl.a \
    ../../lib/common/linux32/libssl.a \
    -L../../lib/public/linux32 -ltier0 \
    -L../../lib/public/linux32 -lvstdlib \
    -L../../lib/public/linux32 -lsteam_api \


LIBFILENAMES = \
    ../../lib/common/linux32/libcrypto.a \
    ../../lib/common/linux32/libcurl.a \
    ../../lib/common/linux32/libcurlssl.a \
    ../../lib/common/linux32/libssl.a \
    ../../lib/public/linux32/bitmap.a \
    ../../lib/public/linux32/choreoobjects.a \
    ../../lib/public/linux32/dmxloader.a \
    ../../lib/public/linux32/libprotobuf.a \
    ../../lib/public/linux32/libsteam_api.so \
    ../../lib/public/linux32/libtier0.so \
    ../../lib/public/linux32/libvstdlib.so \
    ../../lib/public/linux32/libz.a \
    ../../lib/public/linux32/mathlib.a \
    ../../lib/public/linux32/matsys_controls.a \
    ../../lib/public/linux32/particles.a \
    ../../lib/public/linux32/tier1.a \
    ../../lib/public/linux32/tier1.a \
    ../../lib/public/linux32/tier2.a \
    ../../lib/public/linux32/tier3.a \
    ../../lib/public/linux32/vgui_controls.a \
    ../../lib/public/linux32/vtf.a \


# Include the base makefile now.
include $(SRCROOT)/devtools/makefile_base_posix.mak



OTHER_DEPENDENCIES = \


$(OBJ_DIR)/_other_deps.P : $(OTHER_DEPENDENCIES)
	$(GEN_OTHER_DEPS)

-include $(OBJ_DIR)/_other_deps.P



ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/compiledcaptionswap.P
endif

$(OBJ_DIR)/compiledcaptionswap.o : $(PWD)/../../common/compiledcaptionswap.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/language.P
endif

$(OBJ_DIR)/language.o : $(PWD)/../../common/language.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/randoverride.P
endif

$(OBJ_DIR)/randoverride.o : $(PWD)/../../common/randoverride.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_vote_controller.P
endif

$(OBJ_DIR)/c_vote_controller.o : $(PWD)/../../game/client/c_vote_controller.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/achievementmgr.P
endif

$(OBJ_DIR)/achievementmgr.o : $(PWD)/../../game/shared/achievementmgr.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/achievements_hlx.P
endif

$(OBJ_DIR)/achievements_hlx.o : $(PWD)/../../game/shared/achievements_hlx.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/achievement_saverestore.P
endif

$(OBJ_DIR)/achievement_saverestore.o : $(PWD)/../../game/shared/achievement_saverestore.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/activitylist.P
endif

$(OBJ_DIR)/activitylist.o : $(PWD)/../../game/shared/activitylist.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ammodef.P
endif

$(OBJ_DIR)/ammodef.o : $(PWD)/../../game/shared/ammodef.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/animation.P
endif

$(OBJ_DIR)/animation.o : $(PWD)/../../game/shared/animation.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/baseachievement.P
endif

$(OBJ_DIR)/baseachievement.o : $(PWD)/../../game/shared/baseachievement.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/basecombatcharacter_shared.P
endif

$(OBJ_DIR)/basecombatcharacter_shared.o : $(PWD)/../../game/shared/basecombatcharacter_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/basecombatweapon_shared.P
endif

$(OBJ_DIR)/basecombatweapon_shared.o : $(PWD)/../../game/shared/basecombatweapon_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/baseentity_shared.P
endif

$(OBJ_DIR)/baseentity_shared.o : $(PWD)/../../game/shared/baseentity_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/basegrenade_shared.P
endif

$(OBJ_DIR)/basegrenade_shared.o : $(PWD)/../../game/shared/basegrenade_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/baseparticleentity.P
endif

$(OBJ_DIR)/baseparticleentity.o : $(PWD)/../../game/shared/baseparticleentity.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/baseplayer_shared.P
endif

$(OBJ_DIR)/baseplayer_shared.o : $(PWD)/../../game/shared/baseplayer_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/baseprojectile.P
endif

$(OBJ_DIR)/baseprojectile.o : $(PWD)/../../game/shared/baseprojectile.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/baseviewmodel_shared.P
endif

$(OBJ_DIR)/baseviewmodel_shared.o : $(PWD)/../../game/shared/baseviewmodel_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/base_playeranimstate.P
endif

$(OBJ_DIR)/base_playeranimstate.o : $(PWD)/../../game/shared/base_playeranimstate.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/beam_shared.P
endif

$(OBJ_DIR)/beam_shared.o : $(PWD)/../../game/shared/beam_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/cam_thirdperson.P
endif

$(OBJ_DIR)/cam_thirdperson.o : $(PWD)/../../game/shared/cam_thirdperson.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/collisionproperty.P
endif

$(OBJ_DIR)/collisionproperty.o : $(PWD)/../../game/shared/collisionproperty.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/death_pose.P
endif

$(OBJ_DIR)/death_pose.o : $(PWD)/../../game/shared/death_pose.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/debugoverlay_shared.P
endif

$(OBJ_DIR)/debugoverlay_shared.o : $(PWD)/../../game/shared/debugoverlay_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/decals.P
endif

$(OBJ_DIR)/decals.o : $(PWD)/../../game/shared/decals.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/effect_dispatch_data.P
endif

$(OBJ_DIR)/effect_dispatch_data.o : $(PWD)/../../game/shared/effect_dispatch_data.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ehandle.P
endif

$(OBJ_DIR)/ehandle.o : $(PWD)/../../game/shared/ehandle.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/entitylist_base.P
endif

$(OBJ_DIR)/entitylist_base.o : $(PWD)/../../game/shared/entitylist_base.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/EntityParticleTrail_Shared.P
endif

$(OBJ_DIR)/EntityParticleTrail_Shared.o : $(PWD)/../../game/shared/EntityParticleTrail_Shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/env_detail_controller.P
endif

$(OBJ_DIR)/env_detail_controller.o : $(PWD)/../../game/shared/env_detail_controller.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/env_wind_shared.P
endif

$(OBJ_DIR)/env_wind_shared.o : $(PWD)/../../game/shared/env_wind_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/eventlist.P
endif

$(OBJ_DIR)/eventlist.o : $(PWD)/../../game/shared/eventlist.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/func_ladder.P
endif

$(OBJ_DIR)/func_ladder.o : $(PWD)/../../game/shared/func_ladder.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/gamemovement.P
endif

$(OBJ_DIR)/gamemovement.o : $(PWD)/../../game/shared/gamemovement.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/gamerules.P
endif

$(OBJ_DIR)/gamerules.o : $(PWD)/../../game/shared/gamerules.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/gamerules_register.P
endif

$(OBJ_DIR)/gamerules_register.o : $(PWD)/../../game/shared/gamerules_register.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/GameStats.P
endif

$(OBJ_DIR)/GameStats.o : $(PWD)/../../game/shared/GameStats.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/gamestringpool.P
endif

$(OBJ_DIR)/gamestringpool.o : $(PWD)/../../game/shared/gamestringpool.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/gamevars_shared.P
endif

$(OBJ_DIR)/gamevars_shared.o : $(PWD)/../../game/shared/gamevars_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hintmessage.P
endif

$(OBJ_DIR)/hintmessage.o : $(PWD)/../../game/shared/hintmessage.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hintsystem.P
endif

$(OBJ_DIR)/hintsystem.o : $(PWD)/../../game/shared/hintsystem.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/basehlcombatweapon_shared.P
endif

$(OBJ_DIR)/basehlcombatweapon_shared.o : $(PWD)/../../game/shared/hl2/basehlcombatweapon_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/env_headcrabcanister_shared.P
endif

$(OBJ_DIR)/env_headcrabcanister_shared.o : $(PWD)/../../game/shared/hl2/env_headcrabcanister_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hl2_gamerules.P
endif

$(OBJ_DIR)/hl2_gamerules.o : $(PWD)/../../game/shared/hl2/hl2_gamerules.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hl2_usermessages.P
endif

$(OBJ_DIR)/hl2_usermessages.o : $(PWD)/../../game/shared/hl2/hl2_usermessages.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hl_gamemovement.P
endif

$(OBJ_DIR)/hl_gamemovement.o : $(PWD)/../../game/shared/hl2/hl_gamemovement.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hl2mp_gamerules.P
endif

$(OBJ_DIR)/hl2mp_gamerules.o : $(PWD)/../../game/shared/hl2mp/hl2mp_gamerules.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hl2mp_player_shared.P
endif

$(OBJ_DIR)/hl2mp_player_shared.o : $(PWD)/../../game/shared/hl2mp/hl2mp_player_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hl2mp_weapon_parse.P
endif

$(OBJ_DIR)/hl2mp_weapon_parse.o : $(PWD)/../../game/shared/hl2mp/hl2mp_weapon_parse.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/weapon_357.P
endif

$(OBJ_DIR)/weapon_357.o : $(PWD)/../../game/shared/hl2mp/weapon_357.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/weapon_ar2.P
endif

$(OBJ_DIR)/weapon_ar2.o : $(PWD)/../../game/shared/hl2mp/weapon_ar2.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/weapon_crossbow.P
endif

$(OBJ_DIR)/weapon_crossbow.o : $(PWD)/../../game/shared/hl2mp/weapon_crossbow.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/weapon_crowbar.P
endif

$(OBJ_DIR)/weapon_crowbar.o : $(PWD)/../../game/shared/hl2mp/weapon_crowbar.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/weapon_frag.P
endif

$(OBJ_DIR)/weapon_frag.o : $(PWD)/../../game/shared/hl2mp/weapon_frag.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/weapon_hl2mpbase.P
endif

$(OBJ_DIR)/weapon_hl2mpbase.o : $(PWD)/../../game/shared/hl2mp/weapon_hl2mpbase.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/weapon_hl2mpbasebasebludgeon.P
endif

$(OBJ_DIR)/weapon_hl2mpbasebasebludgeon.o : $(PWD)/../../game/shared/hl2mp/weapon_hl2mpbasebasebludgeon.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/weapon_hl2mpbasehlmpcombatweapon.P
endif

$(OBJ_DIR)/weapon_hl2mpbasehlmpcombatweapon.o : $(PWD)/../../game/shared/hl2mp/weapon_hl2mpbasehlmpcombatweapon.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/weapon_hl2mpbase_machinegun.P
endif

$(OBJ_DIR)/weapon_hl2mpbase_machinegun.o : $(PWD)/../../game/shared/hl2mp/weapon_hl2mpbase_machinegun.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/weapon_physcannon.P
endif

$(OBJ_DIR)/weapon_physcannon.o : $(PWD)/../../game/shared/hl2mp/weapon_physcannon.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/weapon_pistol.P
endif

$(OBJ_DIR)/weapon_pistol.o : $(PWD)/../../game/shared/hl2mp/weapon_pistol.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/weapon_rpg.P
endif

$(OBJ_DIR)/weapon_rpg.o : $(PWD)/../../game/shared/hl2mp/weapon_rpg.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/weapon_shotgun.P
endif

$(OBJ_DIR)/weapon_shotgun.o : $(PWD)/../../game/shared/hl2mp/weapon_shotgun.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/weapon_slam.P
endif

$(OBJ_DIR)/weapon_slam.o : $(PWD)/../../game/shared/hl2mp/weapon_slam.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/weapon_smg1.P
endif

$(OBJ_DIR)/weapon_smg1.o : $(PWD)/../../game/shared/hl2mp/weapon_smg1.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/weapon_stunstick.P
endif

$(OBJ_DIR)/weapon_stunstick.o : $(PWD)/../../game/shared/hl2mp/weapon_stunstick.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/igamesystem.P
endif

$(OBJ_DIR)/igamesystem.o : $(PWD)/../../game/shared/igamesystem.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/interval.P
endif

$(OBJ_DIR)/interval.o : $(PWD)/../../game/shared/interval.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/mapentities_shared.P
endif

$(OBJ_DIR)/mapentities_shared.o : $(PWD)/../../game/shared/mapentities_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/movevars_shared.P
endif

$(OBJ_DIR)/movevars_shared.o : $(PWD)/../../game/shared/movevars_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/mp_shareddefs.P
endif

$(OBJ_DIR)/mp_shareddefs.o : $(PWD)/../../game/shared/mp_shareddefs.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/multiplay_gamerules.P
endif

$(OBJ_DIR)/multiplay_gamerules.o : $(PWD)/../../game/shared/multiplay_gamerules.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/obstacle_pushaway.P
endif

$(OBJ_DIR)/obstacle_pushaway.o : $(PWD)/../../game/shared/obstacle_pushaway.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/particlesystemquery.P
endif

$(OBJ_DIR)/particlesystemquery.o : $(PWD)/../../game/shared/particlesystemquery.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/particle_parse.P
endif

$(OBJ_DIR)/particle_parse.o : $(PWD)/../../game/shared/particle_parse.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/particle_property.P
endif

$(OBJ_DIR)/particle_property.o : $(PWD)/../../game/shared/particle_property.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/physics_main_shared.P
endif

$(OBJ_DIR)/physics_main_shared.o : $(PWD)/../../game/shared/physics_main_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/physics_saverestore.P
endif

$(OBJ_DIR)/physics_saverestore.o : $(PWD)/../../game/shared/physics_saverestore.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/physics_shared.P
endif

$(OBJ_DIR)/physics_shared.o : $(PWD)/../../game/shared/physics_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/point_bonusmaps_accessor.P
endif

$(OBJ_DIR)/point_bonusmaps_accessor.o : $(PWD)/../../game/shared/point_bonusmaps_accessor.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/point_posecontroller.P
endif

$(OBJ_DIR)/point_posecontroller.o : $(PWD)/../../game/shared/point_posecontroller.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/precache_register.P
endif

$(OBJ_DIR)/precache_register.o : $(PWD)/../../game/shared/precache_register.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/predictableid.P
endif

$(OBJ_DIR)/predictableid.o : $(PWD)/../../game/shared/predictableid.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/predicted_viewmodel.P
endif

$(OBJ_DIR)/predicted_viewmodel.o : $(PWD)/../../game/shared/predicted_viewmodel.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/predictioncopy.P
endif

$(OBJ_DIR)/predictioncopy.o : $(PWD)/../../game/shared/predictioncopy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/props_shared.P
endif

$(OBJ_DIR)/props_shared.o : $(PWD)/../../game/shared/props_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ragdoll_shared.P
endif

$(OBJ_DIR)/ragdoll_shared.o : $(PWD)/../../game/shared/ragdoll_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/rope_helpers.P
endif

$(OBJ_DIR)/rope_helpers.o : $(PWD)/../../game/shared/rope_helpers.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/saverestore.P
endif

$(OBJ_DIR)/saverestore.o : $(PWD)/../../game/shared/saverestore.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/sceneentity_shared.P
endif

$(OBJ_DIR)/sceneentity_shared.o : $(PWD)/../../game/shared/sceneentity_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/script_intro_shared.P
endif

$(OBJ_DIR)/script_intro_shared.o : $(PWD)/../../game/shared/script_intro_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/sequence_Transitioner.P
endif

$(OBJ_DIR)/sequence_Transitioner.o : $(PWD)/../../game/shared/sequence_Transitioner.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/sheetsimulator.P
endif

$(OBJ_DIR)/sheetsimulator.o : $(PWD)/../../game/shared/sheetsimulator.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/simtimer.P
endif

$(OBJ_DIR)/simtimer.o : $(PWD)/../../game/shared/simtimer.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/singleplay_gamerules.P
endif

$(OBJ_DIR)/singleplay_gamerules.o : $(PWD)/../../game/shared/singleplay_gamerules.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/sixense_convars.P
endif

$(OBJ_DIR)/sixense_convars.o : $(PWD)/../../game/shared/sixense/sixense_convars.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/SoundEmitterSystem.P
endif

$(OBJ_DIR)/SoundEmitterSystem.o : $(PWD)/../../game/shared/SoundEmitterSystem.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/soundenvelope.P
endif

$(OBJ_DIR)/soundenvelope.o : $(PWD)/../../game/shared/soundenvelope.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/Sprite.P
endif

$(OBJ_DIR)/Sprite.o : $(PWD)/../../game/shared/Sprite.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/SpriteTrail.P
endif

$(OBJ_DIR)/SpriteTrail.o : $(PWD)/../../game/shared/SpriteTrail.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/studio_shared.P
endif

$(OBJ_DIR)/studio_shared.o : $(PWD)/../../game/shared/studio_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/takedamageinfo.P
endif

$(OBJ_DIR)/takedamageinfo.o : $(PWD)/../../game/shared/takedamageinfo.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/teamplayroundbased_gamerules.P
endif

$(OBJ_DIR)/teamplayroundbased_gamerules.o : $(PWD)/../../game/shared/teamplayroundbased_gamerules.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/teamplay_gamerules.P
endif

$(OBJ_DIR)/teamplay_gamerules.o : $(PWD)/../../game/shared/teamplay_gamerules.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/teamplay_round_timer.P
endif

$(OBJ_DIR)/teamplay_round_timer.o : $(PWD)/../../game/shared/teamplay_round_timer.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/test_ehandle.P
endif

$(OBJ_DIR)/test_ehandle.o : $(PWD)/../../game/shared/test_ehandle.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/usercmd.P
endif

$(OBJ_DIR)/usercmd.o : $(PWD)/../../game/shared/usercmd.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/usermessages.P
endif

$(OBJ_DIR)/usermessages.o : $(PWD)/../../game/shared/usermessages.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/util_shared.P
endif

$(OBJ_DIR)/util_shared.o : $(PWD)/../../game/shared/util_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vehicle_viewblend_shared.P
endif

$(OBJ_DIR)/vehicle_viewblend_shared.o : $(PWD)/../../game/shared/vehicle_viewblend_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/voice_banmgr.P
endif

$(OBJ_DIR)/voice_banmgr.o : $(PWD)/../../game/shared/voice_banmgr.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/voice_status.P
endif

$(OBJ_DIR)/voice_status.o : $(PWD)/../../game/shared/voice_status.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/weapon_parse.P
endif

$(OBJ_DIR)/weapon_parse.o : $(PWD)/../../game/shared/weapon_parse.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/bone_accessor.P
endif

$(OBJ_DIR)/bone_accessor.o : $(PWD)/../../public/bone_accessor.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/bone_setup.P
endif

$(OBJ_DIR)/bone_setup.o : $(PWD)/../../public/bone_setup.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/client_class.P
endif

$(OBJ_DIR)/client_class.o : $(PWD)/../../public/client_class.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/collisionutils.P
endif

$(OBJ_DIR)/collisionutils.o : $(PWD)/../../public/collisionutils.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/crtmemdebug.P
endif

$(OBJ_DIR)/crtmemdebug.o : $(PWD)/../../public/crtmemdebug.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/dt_recv.P
endif

$(OBJ_DIR)/dt_recv.o : $(PWD)/../../public/dt_recv.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/dt_utlvector_common.P
endif

$(OBJ_DIR)/dt_utlvector_common.o : $(PWD)/../../public/dt_utlvector_common.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/dt_utlvector_recv.P
endif

$(OBJ_DIR)/dt_utlvector_recv.o : $(PWD)/../../public/dt_utlvector_recv.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/filesystem_helpers.P
endif

$(OBJ_DIR)/filesystem_helpers.o : $(PWD)/../../public/filesystem_helpers.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/haptic_msgs.P
endif

$(OBJ_DIR)/haptic_msgs.o : $(PWD)/../../public/haptics/haptic_msgs.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/interpolatortypes.P
endif

$(OBJ_DIR)/interpolatortypes.o : $(PWD)/../../public/interpolatortypes.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/jigglebones.P
endif

$(OBJ_DIR)/jigglebones.o : $(PWD)/../../public/jigglebones.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/networkvar.P
endif

$(OBJ_DIR)/networkvar.o : $(PWD)/../../public/networkvar.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/posedebugger.P
endif

$(OBJ_DIR)/posedebugger.o : $(PWD)/../../public/posedebugger.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/renamed_recvtable_compat.P
endif

$(OBJ_DIR)/renamed_recvtable_compat.o : $(PWD)/../../public/renamed_recvtable_compat.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/rope_physics.P
endif

$(OBJ_DIR)/rope_physics.o : $(PWD)/../../public/rope_physics.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/scratchpad3d.P
endif

$(OBJ_DIR)/scratchpad3d.o : $(PWD)/../../public/scratchpad3d.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ScratchPadUtils.P
endif

$(OBJ_DIR)/ScratchPadUtils.o : $(PWD)/../../public/ScratchPadUtils.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/sentence.P
endif

$(OBJ_DIR)/sentence.o : $(PWD)/../../public/sentence.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/simple_physics.P
endif

$(OBJ_DIR)/simple_physics.o : $(PWD)/../../public/simple_physics.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/SoundParametersInternal.P
endif

$(OBJ_DIR)/SoundParametersInternal.o : $(PWD)/../../public/SoundParametersInternal.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/stringregistry.P
endif

$(OBJ_DIR)/stringregistry.o : $(PWD)/../../public/stringregistry.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/studio.P
endif

$(OBJ_DIR)/studio.o : $(PWD)/../../public/studio.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/memoverride.P
endif

$(OBJ_DIR)/memoverride.o : $(PWD)/../../public/tier0/memoverride.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/bonelist.P
endif

$(OBJ_DIR)/bonelist.o : $(PWD)/../../public/tools/bonelist.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vallocator.P
endif

$(OBJ_DIR)/vallocator.o : $(PWD)/../../public/vallocator.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_controls.P
endif

$(OBJ_DIR)/vgui_controls.o : $(PWD)/../../public/vgui_controls/vgui_controls.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/achievement_notification_panel.P
endif

$(OBJ_DIR)/achievement_notification_panel.o : $(PWD)/achievement_notification_panel.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/alphamaterialproxy.P
endif

$(OBJ_DIR)/alphamaterialproxy.o : $(PWD)/alphamaterialproxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/animatedentitytextureproxy.P
endif

$(OBJ_DIR)/animatedentitytextureproxy.o : $(PWD)/animatedentitytextureproxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/animatedoffsettextureproxy.P
endif

$(OBJ_DIR)/animatedoffsettextureproxy.o : $(PWD)/animatedoffsettextureproxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/animatedtextureproxy.P
endif

$(OBJ_DIR)/animatedtextureproxy.o : $(PWD)/animatedtextureproxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/AnimateSpecificTextureProxy.P
endif

$(OBJ_DIR)/AnimateSpecificTextureProxy.o : $(PWD)/AnimateSpecificTextureProxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/baseanimatedtextureproxy.P
endif

$(OBJ_DIR)/baseanimatedtextureproxy.o : $(PWD)/baseanimatedtextureproxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/baseclientrendertargets.P
endif

$(OBJ_DIR)/baseclientrendertargets.o : $(PWD)/baseclientrendertargets.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/basepresence.P
endif

$(OBJ_DIR)/basepresence.o : $(PWD)/basepresence.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/beamdraw.P
endif

$(OBJ_DIR)/beamdraw.o : $(PWD)/beamdraw.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/bone_merge_cache.P
endif

$(OBJ_DIR)/bone_merge_cache.o : $(PWD)/bone_merge_cache.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/camomaterialproxy.P
endif

$(OBJ_DIR)/camomaterialproxy.o : $(PWD)/camomaterialproxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/cdll_bounded_cvars.P
endif

$(OBJ_DIR)/cdll_bounded_cvars.o : $(PWD)/cdll_bounded_cvars.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/cdll_client_int.P
endif

$(OBJ_DIR)/cdll_client_int.o : $(PWD)/cdll_client_int.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/cdll_util.P
endif

$(OBJ_DIR)/cdll_util.o : $(PWD)/cdll_util.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/classmap.P
endif

$(OBJ_DIR)/classmap.o : $(PWD)/classmap.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/clienteffectprecachesystem.P
endif

$(OBJ_DIR)/clienteffectprecachesystem.o : $(PWD)/clienteffectprecachesystem.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/cliententitylist.P
endif

$(OBJ_DIR)/cliententitylist.o : $(PWD)/cliententitylist.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/clientleafsystem.P
endif

$(OBJ_DIR)/clientleafsystem.o : $(PWD)/clientleafsystem.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/clientmode_shared.P
endif

$(OBJ_DIR)/clientmode_shared.o : $(PWD)/clientmode_shared.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/clientshadowmgr.P
endif

$(OBJ_DIR)/clientshadowmgr.o : $(PWD)/clientshadowmgr.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/clientsideeffects.P
endif

$(OBJ_DIR)/clientsideeffects.o : $(PWD)/clientsideeffects.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/clientsideeffects_test.P
endif

$(OBJ_DIR)/clientsideeffects_test.o : $(PWD)/clientsideeffects_test.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/clientsteamcontext.P
endif

$(OBJ_DIR)/clientsteamcontext.o : $(PWD)/clientsteamcontext.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/client_factorylist.P
endif

$(OBJ_DIR)/client_factorylist.o : $(PWD)/client_factorylist.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/client_thinklist.P
endif

$(OBJ_DIR)/client_thinklist.o : $(PWD)/client_thinklist.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/client_virtualreality.P
endif

$(OBJ_DIR)/client_virtualreality.o : $(PWD)/client_virtualreality.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/cl_mat_stub.P
endif

$(OBJ_DIR)/cl_mat_stub.o : $(PWD)/cl_mat_stub.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/colorcorrectionmgr.P
endif

$(OBJ_DIR)/colorcorrectionmgr.o : $(PWD)/colorcorrectionmgr.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/commentary_modelviewer.P
endif

$(OBJ_DIR)/commentary_modelviewer.o : $(PWD)/commentary_modelviewer.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_ai_basehumanoid.P
endif

$(OBJ_DIR)/c_ai_basehumanoid.o : $(PWD)/c_ai_basehumanoid.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_ai_basenpc.P
endif

$(OBJ_DIR)/c_ai_basenpc.o : $(PWD)/c_ai_basenpc.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_baseanimating.P
endif

$(OBJ_DIR)/c_baseanimating.o : $(PWD)/c_baseanimating.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_baseanimatingoverlay.P
endif

$(OBJ_DIR)/c_baseanimatingoverlay.o : $(PWD)/c_baseanimatingoverlay.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_basecombatcharacter.P
endif

$(OBJ_DIR)/c_basecombatcharacter.o : $(PWD)/c_basecombatcharacter.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_basecombatweapon.P
endif

$(OBJ_DIR)/c_basecombatweapon.o : $(PWD)/c_basecombatweapon.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_basedoor.P
endif

$(OBJ_DIR)/c_basedoor.o : $(PWD)/c_basedoor.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_baseentity.P
endif

$(OBJ_DIR)/c_baseentity.o : $(PWD)/c_baseentity.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_baseflex.P
endif

$(OBJ_DIR)/c_baseflex.o : $(PWD)/c_baseflex.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_baseplayer.P
endif

$(OBJ_DIR)/c_baseplayer.o : $(PWD)/c_baseplayer.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_basetempentity.P
endif

$(OBJ_DIR)/c_basetempentity.o : $(PWD)/c_basetempentity.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_baseviewmodel.P
endif

$(OBJ_DIR)/c_baseviewmodel.o : $(PWD)/c_baseviewmodel.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_breakableprop.P
endif

$(OBJ_DIR)/c_breakableprop.o : $(PWD)/c_breakableprop.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_colorcorrection.P
endif

$(OBJ_DIR)/c_colorcorrection.o : $(PWD)/c_colorcorrection.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_colorcorrectionvolume.P
endif

$(OBJ_DIR)/c_colorcorrectionvolume.o : $(PWD)/c_colorcorrectionvolume.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_dynamiclight.P
endif

$(OBJ_DIR)/c_dynamiclight.o : $(PWD)/c_dynamiclight.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_effects.P
endif

$(OBJ_DIR)/c_effects.o : $(PWD)/c_effects.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_entitydissolve.P
endif

$(OBJ_DIR)/c_entitydissolve.o : $(PWD)/c_entitydissolve.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_entityparticletrail.P
endif

$(OBJ_DIR)/c_entityparticletrail.o : $(PWD)/c_entityparticletrail.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_env_fog_controller.P
endif

$(OBJ_DIR)/c_env_fog_controller.o : $(PWD)/c_env_fog_controller.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_env_particlescript.P
endif

$(OBJ_DIR)/c_env_particlescript.o : $(PWD)/c_env_particlescript.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_env_projectedtexture.P
endif

$(OBJ_DIR)/c_env_projectedtexture.o : $(PWD)/c_env_projectedtexture.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_env_screenoverlay.P
endif

$(OBJ_DIR)/c_env_screenoverlay.o : $(PWD)/c_env_screenoverlay.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_env_tonemap_controller.P
endif

$(OBJ_DIR)/c_env_tonemap_controller.o : $(PWD)/c_env_tonemap_controller.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_fire_smoke.P
endif

$(OBJ_DIR)/c_fire_smoke.o : $(PWD)/c_fire_smoke.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_fish.P
endif

$(OBJ_DIR)/c_fish.o : $(PWD)/c_fish.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_func_areaportalwindow.P
endif

$(OBJ_DIR)/c_func_areaportalwindow.o : $(PWD)/c_func_areaportalwindow.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_func_breakablesurf.P
endif

$(OBJ_DIR)/c_func_breakablesurf.o : $(PWD)/c_func_breakablesurf.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_func_conveyor.P
endif

$(OBJ_DIR)/c_func_conveyor.o : $(PWD)/c_func_conveyor.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_func_dust.P
endif

$(OBJ_DIR)/c_func_dust.o : $(PWD)/c_func_dust.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_func_lod.P
endif

$(OBJ_DIR)/c_func_lod.o : $(PWD)/c_func_lod.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_func_occluder.P
endif

$(OBJ_DIR)/c_func_occluder.o : $(PWD)/c_func_occluder.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_func_reflective_glass.P
endif

$(OBJ_DIR)/c_func_reflective_glass.o : $(PWD)/c_func_reflective_glass.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_func_rotating.P
endif

$(OBJ_DIR)/c_func_rotating.o : $(PWD)/c_func_rotating.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_func_smokevolume.P
endif

$(OBJ_DIR)/c_func_smokevolume.o : $(PWD)/c_func_smokevolume.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_func_tracktrain.P
endif

$(OBJ_DIR)/c_func_tracktrain.o : $(PWD)/c_func_tracktrain.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_gib.P
endif

$(OBJ_DIR)/c_gib.o : $(PWD)/c_gib.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_hairball.P
endif

$(OBJ_DIR)/c_hairball.o : $(PWD)/c_hairball.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_impact_effects.P
endif

$(OBJ_DIR)/c_impact_effects.o : $(PWD)/c_impact_effects.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_info_overlay_accessor.P
endif

$(OBJ_DIR)/c_info_overlay_accessor.o : $(PWD)/c_info_overlay_accessor.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_lightglow.P
endif

$(OBJ_DIR)/c_lightglow.o : $(PWD)/c_lightglow.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/C_MaterialModifyControl.P
endif

$(OBJ_DIR)/C_MaterialModifyControl.o : $(PWD)/C_MaterialModifyControl.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_movie_explosion.P
endif

$(OBJ_DIR)/c_movie_explosion.o : $(PWD)/c_movie_explosion.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_particle_fire.P
endif

$(OBJ_DIR)/c_particle_fire.o : $(PWD)/c_particle_fire.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_particle_smokegrenade.P
endif

$(OBJ_DIR)/c_particle_smokegrenade.o : $(PWD)/c_particle_smokegrenade.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_particle_system.P
endif

$(OBJ_DIR)/c_particle_system.o : $(PWD)/c_particle_system.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_physbox.P
endif

$(OBJ_DIR)/c_physbox.o : $(PWD)/c_physbox.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_physicsprop.P
endif

$(OBJ_DIR)/c_physicsprop.o : $(PWD)/c_physicsprop.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_physmagnet.P
endif

$(OBJ_DIR)/c_physmagnet.o : $(PWD)/c_physmagnet.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_pixel_visibility.P
endif

$(OBJ_DIR)/c_pixel_visibility.o : $(PWD)/c_pixel_visibility.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_plasma.P
endif

$(OBJ_DIR)/c_plasma.o : $(PWD)/c_plasma.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_playerresource.P
endif

$(OBJ_DIR)/c_playerresource.o : $(PWD)/c_playerresource.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_point_camera.P
endif

$(OBJ_DIR)/c_point_camera.o : $(PWD)/c_point_camera.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_point_commentary_node.P
endif

$(OBJ_DIR)/c_point_commentary_node.o : $(PWD)/c_point_commentary_node.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_props.P
endif

$(OBJ_DIR)/c_props.o : $(PWD)/c_props.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_prop_vehicle.P
endif

$(OBJ_DIR)/c_prop_vehicle.o : $(PWD)/c_prop_vehicle.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_ragdoll_manager.P
endif

$(OBJ_DIR)/c_ragdoll_manager.o : $(PWD)/c_ragdoll_manager.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_recipientfilter.P
endif

$(OBJ_DIR)/c_recipientfilter.o : $(PWD)/c_recipientfilter.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_rope.P
endif

$(OBJ_DIR)/c_rope.o : $(PWD)/c_rope.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_rumble.P
endif

$(OBJ_DIR)/c_rumble.o : $(PWD)/c_rumble.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_sceneentity.P
endif

$(OBJ_DIR)/c_sceneentity.o : $(PWD)/c_sceneentity.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_shadowcontrol.P
endif

$(OBJ_DIR)/c_shadowcontrol.o : $(PWD)/c_shadowcontrol.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_slideshow_display.P
endif

$(OBJ_DIR)/c_slideshow_display.o : $(PWD)/c_slideshow_display.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_smokestack.P
endif

$(OBJ_DIR)/c_smokestack.o : $(PWD)/c_smokestack.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_smoke_trail.P
endif

$(OBJ_DIR)/c_smoke_trail.o : $(PWD)/c_smoke_trail.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_soundscape.P
endif

$(OBJ_DIR)/c_soundscape.o : $(PWD)/c_soundscape.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_spotlight_end.P
endif

$(OBJ_DIR)/c_spotlight_end.o : $(PWD)/c_spotlight_end.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_sprite.P
endif

$(OBJ_DIR)/c_sprite.o : $(PWD)/c_sprite.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_sprite_perfmonitor.P
endif

$(OBJ_DIR)/c_sprite_perfmonitor.o : $(PWD)/c_sprite_perfmonitor.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_steamjet.P
endif

$(OBJ_DIR)/c_steamjet.o : $(PWD)/c_steamjet.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_stickybolt.P
endif

$(OBJ_DIR)/c_stickybolt.o : $(PWD)/c_stickybolt.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_sun.P
endif

$(OBJ_DIR)/c_sun.o : $(PWD)/c_sun.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te.P
endif

$(OBJ_DIR)/c_te.o : $(PWD)/c_te.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_team.P
endif

$(OBJ_DIR)/c_team.o : $(PWD)/c_team.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_team_objectiveresource.P
endif

$(OBJ_DIR)/c_team_objectiveresource.o : $(PWD)/c_team_objectiveresource.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_team_train_watcher.P
endif

$(OBJ_DIR)/c_team_train_watcher.o : $(PWD)/c_team_train_watcher.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_tesla.P
endif

$(OBJ_DIR)/c_tesla.o : $(PWD)/c_tesla.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_testtraceline.P
endif

$(OBJ_DIR)/c_testtraceline.o : $(PWD)/c_testtraceline.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_test_proxytoggle.P
endif

$(OBJ_DIR)/c_test_proxytoggle.o : $(PWD)/c_test_proxytoggle.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_armorricochet.P
endif

$(OBJ_DIR)/c_te_armorricochet.o : $(PWD)/c_te_armorricochet.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_basebeam.P
endif

$(OBJ_DIR)/c_te_basebeam.o : $(PWD)/c_te_basebeam.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_beamentpoint.P
endif

$(OBJ_DIR)/c_te_beamentpoint.o : $(PWD)/c_te_beamentpoint.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_beaments.P
endif

$(OBJ_DIR)/c_te_beaments.o : $(PWD)/c_te_beaments.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_beamfollow.P
endif

$(OBJ_DIR)/c_te_beamfollow.o : $(PWD)/c_te_beamfollow.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_beamlaser.P
endif

$(OBJ_DIR)/c_te_beamlaser.o : $(PWD)/c_te_beamlaser.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_beampoints.P
endif

$(OBJ_DIR)/c_te_beampoints.o : $(PWD)/c_te_beampoints.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_beamring.P
endif

$(OBJ_DIR)/c_te_beamring.o : $(PWD)/c_te_beamring.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_beamringpoint.P
endif

$(OBJ_DIR)/c_te_beamringpoint.o : $(PWD)/c_te_beamringpoint.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_beamspline.P
endif

$(OBJ_DIR)/c_te_beamspline.o : $(PWD)/c_te_beamspline.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_bloodsprite.P
endif

$(OBJ_DIR)/c_te_bloodsprite.o : $(PWD)/c_te_bloodsprite.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_bloodstream.P
endif

$(OBJ_DIR)/c_te_bloodstream.o : $(PWD)/c_te_bloodstream.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_breakmodel.P
endif

$(OBJ_DIR)/c_te_breakmodel.o : $(PWD)/c_te_breakmodel.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_bspdecal.P
endif

$(OBJ_DIR)/c_te_bspdecal.o : $(PWD)/c_te_bspdecal.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_bubbles.P
endif

$(OBJ_DIR)/c_te_bubbles.o : $(PWD)/c_te_bubbles.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_bubbletrail.P
endif

$(OBJ_DIR)/c_te_bubbletrail.o : $(PWD)/c_te_bubbletrail.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_clientprojectile.P
endif

$(OBJ_DIR)/c_te_clientprojectile.o : $(PWD)/c_te_clientprojectile.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_decal.P
endif

$(OBJ_DIR)/c_te_decal.o : $(PWD)/c_te_decal.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_dynamiclight.P
endif

$(OBJ_DIR)/c_te_dynamiclight.o : $(PWD)/c_te_dynamiclight.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_effect_dispatch.P
endif

$(OBJ_DIR)/c_te_effect_dispatch.o : $(PWD)/c_te_effect_dispatch.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_energysplash.P
endif

$(OBJ_DIR)/c_te_energysplash.o : $(PWD)/c_te_energysplash.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_explosion.P
endif

$(OBJ_DIR)/c_te_explosion.o : $(PWD)/c_te_explosion.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_fizz.P
endif

$(OBJ_DIR)/c_te_fizz.o : $(PWD)/c_te_fizz.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_footprint.P
endif

$(OBJ_DIR)/c_te_footprint.o : $(PWD)/c_te_footprint.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_glassshatter.P
endif

$(OBJ_DIR)/c_te_glassshatter.o : $(PWD)/c_te_glassshatter.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_glowsprite.P
endif

$(OBJ_DIR)/c_te_glowsprite.o : $(PWD)/c_te_glowsprite.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_impact.P
endif

$(OBJ_DIR)/c_te_impact.o : $(PWD)/c_te_impact.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_killplayerattachments.P
endif

$(OBJ_DIR)/c_te_killplayerattachments.o : $(PWD)/c_te_killplayerattachments.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_largefunnel.P
endif

$(OBJ_DIR)/c_te_largefunnel.o : $(PWD)/c_te_largefunnel.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_legacytempents.P
endif

$(OBJ_DIR)/c_te_legacytempents.o : $(PWD)/c_te_legacytempents.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_muzzleflash.P
endif

$(OBJ_DIR)/c_te_muzzleflash.o : $(PWD)/c_te_muzzleflash.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_particlesystem.P
endif

$(OBJ_DIR)/c_te_particlesystem.o : $(PWD)/c_te_particlesystem.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_physicsprop.P
endif

$(OBJ_DIR)/c_te_physicsprop.o : $(PWD)/c_te_physicsprop.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_playerdecal.P
endif

$(OBJ_DIR)/c_te_playerdecal.o : $(PWD)/c_te_playerdecal.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_projecteddecal.P
endif

$(OBJ_DIR)/c_te_projecteddecal.o : $(PWD)/c_te_projecteddecal.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_showline.P
endif

$(OBJ_DIR)/c_te_showline.o : $(PWD)/c_te_showline.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_smoke.P
endif

$(OBJ_DIR)/c_te_smoke.o : $(PWD)/c_te_smoke.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_sparks.P
endif

$(OBJ_DIR)/c_te_sparks.o : $(PWD)/c_te_sparks.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_sprite.P
endif

$(OBJ_DIR)/c_te_sprite.o : $(PWD)/c_te_sprite.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_spritespray.P
endif

$(OBJ_DIR)/c_te_spritespray.o : $(PWD)/c_te_spritespray.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_worlddecal.P
endif

$(OBJ_DIR)/c_te_worlddecal.o : $(PWD)/c_te_worlddecal.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_tracer.P
endif

$(OBJ_DIR)/c_tracer.o : $(PWD)/c_tracer.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_user_message_register.P
endif

$(OBJ_DIR)/c_user_message_register.o : $(PWD)/c_user_message_register.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_vehicle_choreo_generic.P
endif

$(OBJ_DIR)/c_vehicle_choreo_generic.o : $(PWD)/c_vehicle_choreo_generic.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_vehicle_jeep.P
endif

$(OBJ_DIR)/c_vehicle_jeep.o : $(PWD)/c_vehicle_jeep.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_vguiscreen.P
endif

$(OBJ_DIR)/c_vguiscreen.o : $(PWD)/c_vguiscreen.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/C_WaterLODControl.P
endif

$(OBJ_DIR)/C_WaterLODControl.o : $(PWD)/C_WaterLODControl.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_world.P
endif

$(OBJ_DIR)/c_world.o : $(PWD)/c_world.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/detailobjectsystem.P
endif

$(OBJ_DIR)/detailobjectsystem.o : $(PWD)/detailobjectsystem.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/dummyproxy.P
endif

$(OBJ_DIR)/dummyproxy.o : $(PWD)/dummyproxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/EffectsClient.P
endif

$(OBJ_DIR)/EffectsClient.o : $(PWD)/EffectsClient.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/entityoriginmaterialproxy.P
endif

$(OBJ_DIR)/entityoriginmaterialproxy.o : $(PWD)/entityoriginmaterialproxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/entity_client_tools.P
endif

$(OBJ_DIR)/entity_client_tools.o : $(PWD)/entity_client_tools.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_vort_charge_token.P
endif

$(OBJ_DIR)/c_vort_charge_token.o : $(PWD)/episodic/c_vort_charge_token.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/flashlighteffect.P
endif

$(OBJ_DIR)/flashlighteffect.o : $(PWD)/flashlighteffect.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/functionproxy.P
endif

$(OBJ_DIR)/functionproxy.o : $(PWD)/functionproxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx.P
endif

$(OBJ_DIR)/fx.o : $(PWD)/fx.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx_blood.P
endif

$(OBJ_DIR)/fx_blood.o : $(PWD)/fx_blood.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx_cube.P
endif

$(OBJ_DIR)/fx_cube.o : $(PWD)/fx_cube.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx_discreetline.P
endif

$(OBJ_DIR)/fx_discreetline.o : $(PWD)/fx_discreetline.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx_envelope.P
endif

$(OBJ_DIR)/fx_envelope.o : $(PWD)/fx_envelope.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx_explosion.P
endif

$(OBJ_DIR)/fx_explosion.o : $(PWD)/fx_explosion.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx_fleck.P
endif

$(OBJ_DIR)/fx_fleck.o : $(PWD)/fx_fleck.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx_impact.P
endif

$(OBJ_DIR)/fx_impact.o : $(PWD)/fx_impact.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx_interpvalue.P
endif

$(OBJ_DIR)/fx_interpvalue.o : $(PWD)/fx_interpvalue.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx_line.P
endif

$(OBJ_DIR)/fx_line.o : $(PWD)/fx_line.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx_quad.P
endif

$(OBJ_DIR)/fx_quad.o : $(PWD)/fx_quad.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx_shelleject.P
endif

$(OBJ_DIR)/fx_shelleject.o : $(PWD)/fx_shelleject.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx_sparks.P
endif

$(OBJ_DIR)/fx_sparks.o : $(PWD)/fx_sparks.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx_staticline.P
endif

$(OBJ_DIR)/fx_staticline.o : $(PWD)/fx_staticline.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx_tracer.P
endif

$(OBJ_DIR)/fx_tracer.o : $(PWD)/fx_tracer.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx_trail.P
endif

$(OBJ_DIR)/fx_trail.o : $(PWD)/fx_trail.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx_water.P
endif

$(OBJ_DIR)/fx_water.o : $(PWD)/fx_water.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/gametrace_client.P
endif

$(OBJ_DIR)/gametrace_client.o : $(PWD)/gametrace_client.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/basemodelpanel.P
endif

$(OBJ_DIR)/basemodelpanel.o : $(PWD)/game_controls/basemodelpanel.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/basemodel_panel.P
endif

$(OBJ_DIR)/basemodel_panel.o : $(PWD)/game_controls/basemodel_panel.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/baseviewport.P
endif

$(OBJ_DIR)/baseviewport.o : $(PWD)/game_controls/baseviewport.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ClientScoreBoardDialog.P
endif

$(OBJ_DIR)/ClientScoreBoardDialog.o : $(PWD)/game_controls/ClientScoreBoardDialog.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/commandmenu.P
endif

$(OBJ_DIR)/commandmenu.o : $(PWD)/game_controls/commandmenu.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/IconPanel.P
endif

$(OBJ_DIR)/IconPanel.o : $(PWD)/game_controls/IconPanel.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/intromenu.P
endif

$(OBJ_DIR)/intromenu.o : $(PWD)/game_controls/intromenu.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/MapOverview.P
endif

$(OBJ_DIR)/MapOverview.o : $(PWD)/game_controls/MapOverview.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/NavProgress.P
endif

$(OBJ_DIR)/NavProgress.o : $(PWD)/game_controls/NavProgress.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/SpectatorGUI.P
endif

$(OBJ_DIR)/SpectatorGUI.o : $(PWD)/game_controls/SpectatorGUI.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/teammenu.P
endif

$(OBJ_DIR)/teammenu.o : $(PWD)/game_controls/teammenu.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vguitextwindow.P
endif

$(OBJ_DIR)/vguitextwindow.o : $(PWD)/game_controls/vguitextwindow.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/geiger.P
endif

$(OBJ_DIR)/geiger.o : $(PWD)/geiger.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/glow_outline_effect.P
endif

$(OBJ_DIR)/glow_outline_effect.o : $(PWD)/glow_outline_effect.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/glow_overlay.P
endif

$(OBJ_DIR)/glow_overlay.o : $(PWD)/glow_overlay.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/history_resource.P
endif

$(OBJ_DIR)/history_resource.o : $(PWD)/history_resource.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_antlion_dust.P
endif

$(OBJ_DIR)/c_antlion_dust.o : $(PWD)/hl2/c_antlion_dust.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_ar2_explosion.P
endif

$(OBJ_DIR)/c_ar2_explosion.o : $(PWD)/hl2/c_ar2_explosion.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_barnacle.P
endif

$(OBJ_DIR)/c_barnacle.o : $(PWD)/hl2/c_barnacle.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_barney.P
endif

$(OBJ_DIR)/c_barney.o : $(PWD)/hl2/c_barney.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_basehelicopter.P
endif

$(OBJ_DIR)/c_basehelicopter.o : $(PWD)/hl2/c_basehelicopter.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_basehlcombatweapon.P
endif

$(OBJ_DIR)/c_basehlcombatweapon.o : $(PWD)/hl2/c_basehlcombatweapon.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_basehlplayer.P
endif

$(OBJ_DIR)/c_basehlplayer.o : $(PWD)/hl2/c_basehlplayer.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_citadel_effects.P
endif

$(OBJ_DIR)/c_citadel_effects.o : $(PWD)/hl2/c_citadel_effects.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_corpse.P
endif

$(OBJ_DIR)/c_corpse.o : $(PWD)/hl2/c_corpse.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_env_alyxtemp.P
endif

$(OBJ_DIR)/c_env_alyxtemp.o : $(PWD)/hl2/c_env_alyxtemp.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_env_headcrabcanister.P
endif

$(OBJ_DIR)/c_env_headcrabcanister.o : $(PWD)/hl2/c_env_headcrabcanister.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_env_starfield.P
endif

$(OBJ_DIR)/c_env_starfield.o : $(PWD)/hl2/c_env_starfield.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/C_Func_Monitor.P
endif

$(OBJ_DIR)/C_Func_Monitor.o : $(PWD)/hl2/C_Func_Monitor.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_func_tankmortar.P
endif

$(OBJ_DIR)/c_func_tankmortar.o : $(PWD)/hl2/c_func_tankmortar.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_hl2_playerlocaldata.P
endif

$(OBJ_DIR)/c_hl2_playerlocaldata.o : $(PWD)/hl2/c_hl2_playerlocaldata.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_info_teleporter_countdown.P
endif

$(OBJ_DIR)/c_info_teleporter_countdown.o : $(PWD)/hl2/c_info_teleporter_countdown.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_npc_antlionguard.P
endif

$(OBJ_DIR)/c_npc_antlionguard.o : $(PWD)/hl2/c_npc_antlionguard.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_npc_combinegunship.P
endif

$(OBJ_DIR)/c_npc_combinegunship.o : $(PWD)/hl2/c_npc_combinegunship.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_npc_manhack.P
endif

$(OBJ_DIR)/c_npc_manhack.o : $(PWD)/hl2/c_npc_manhack.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_npc_rollermine.P
endif

$(OBJ_DIR)/c_npc_rollermine.o : $(PWD)/hl2/c_npc_rollermine.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_plasma_beam_node.P
endif

$(OBJ_DIR)/c_plasma_beam_node.o : $(PWD)/hl2/c_plasma_beam_node.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_prop_combine_ball.P
endif

$(OBJ_DIR)/c_prop_combine_ball.o : $(PWD)/hl2/c_prop_combine_ball.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_rotorwash.P
endif

$(OBJ_DIR)/c_rotorwash.o : $(PWD)/hl2/c_rotorwash.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_script_intro.P
endif

$(OBJ_DIR)/c_script_intro.o : $(PWD)/hl2/c_script_intro.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_strider.P
endif

$(OBJ_DIR)/c_strider.o : $(PWD)/hl2/c_strider.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_concussiveexplosion.P
endif

$(OBJ_DIR)/c_te_concussiveexplosion.o : $(PWD)/hl2/c_te_concussiveexplosion.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_flare.P
endif

$(OBJ_DIR)/c_te_flare.o : $(PWD)/hl2/c_te_flare.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_thumper_dust.P
endif

$(OBJ_DIR)/c_thumper_dust.o : $(PWD)/hl2/c_thumper_dust.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_vehicle_airboat.P
endif

$(OBJ_DIR)/c_vehicle_airboat.o : $(PWD)/hl2/c_vehicle_airboat.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_vehicle_cannon.P
endif

$(OBJ_DIR)/c_vehicle_cannon.o : $(PWD)/hl2/c_vehicle_cannon.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_vehicle_crane.P
endif

$(OBJ_DIR)/c_vehicle_crane.o : $(PWD)/hl2/c_vehicle_crane.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_vehicle_prisoner_pod.P
endif

$(OBJ_DIR)/c_vehicle_prisoner_pod.o : $(PWD)/hl2/c_vehicle_prisoner_pod.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_waterbullet.P
endif

$(OBJ_DIR)/c_waterbullet.o : $(PWD)/hl2/c_waterbullet.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_weapon_crossbow.P
endif

$(OBJ_DIR)/c_weapon_crossbow.o : $(PWD)/hl2/c_weapon_crossbow.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_weapon__stubs_hl2.P
endif

$(OBJ_DIR)/c_weapon__stubs_hl2.o : $(PWD)/hl2/c_weapon__stubs_hl2.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx_antlion.P
endif

$(OBJ_DIR)/fx_antlion.o : $(PWD)/hl2/fx_antlion.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx_bugbait.P
endif

$(OBJ_DIR)/fx_bugbait.o : $(PWD)/hl2/fx_bugbait.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx_hl2_impacts.P
endif

$(OBJ_DIR)/fx_hl2_impacts.o : $(PWD)/hl2/fx_hl2_impacts.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/fx_hl2_tracers.P
endif

$(OBJ_DIR)/fx_hl2_tracers.o : $(PWD)/hl2/fx_hl2_tracers.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hl2_clientmode.P
endif

$(OBJ_DIR)/hl2_clientmode.o : $(PWD)/hl2/hl2_clientmode.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hl_in_main.P
endif

$(OBJ_DIR)/hl_in_main.o : $(PWD)/hl2/hl_in_main.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hl_prediction.P
endif

$(OBJ_DIR)/hl_prediction.o : $(PWD)/hl2/hl_prediction.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_ammo.P
endif

$(OBJ_DIR)/hud_ammo.o : $(PWD)/hl2/hud_ammo.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_autoaim.P
endif

$(OBJ_DIR)/hud_autoaim.o : $(PWD)/hl2/hud_autoaim.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_battery.P
endif

$(OBJ_DIR)/hud_battery.o : $(PWD)/hl2/hud_battery.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_blood.P
endif

$(OBJ_DIR)/hud_blood.o : $(PWD)/hl2/hud_blood.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_credits.P
endif

$(OBJ_DIR)/hud_credits.o : $(PWD)/hl2/hud_credits.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_damageindicator.P
endif

$(OBJ_DIR)/hud_damageindicator.o : $(PWD)/hl2/hud_damageindicator.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_filmdemo.P
endif

$(OBJ_DIR)/hud_filmdemo.o : $(PWD)/hl2/hud_filmdemo.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_flashlight.P
endif

$(OBJ_DIR)/hud_flashlight.o : $(PWD)/hl2/hud_flashlight.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_hdrdemo.P
endif

$(OBJ_DIR)/hud_hdrdemo.o : $(PWD)/hl2/hud_hdrdemo.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_health.P
endif

$(OBJ_DIR)/hud_health.o : $(PWD)/hl2/hud_health.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_poisondamageindicator.P
endif

$(OBJ_DIR)/hud_poisondamageindicator.o : $(PWD)/hl2/hud_poisondamageindicator.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_quickinfo.P
endif

$(OBJ_DIR)/hud_quickinfo.o : $(PWD)/hl2/hud_quickinfo.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_suitpower.P
endif

$(OBJ_DIR)/hud_suitpower.o : $(PWD)/hl2/hud_suitpower.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_weaponselection.P
endif

$(OBJ_DIR)/hud_weaponselection.o : $(PWD)/hl2/hud_weaponselection.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_zoom.P
endif

$(OBJ_DIR)/hud_zoom.o : $(PWD)/hl2/hud_zoom.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/shieldproxy.P
endif

$(OBJ_DIR)/shieldproxy.o : $(PWD)/hl2/shieldproxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_rootpanel_hl2.P
endif

$(OBJ_DIR)/vgui_rootpanel_hl2.o : $(PWD)/hl2/vgui_rootpanel_hl2.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/clientmode_hl2mpnormal.P
endif

$(OBJ_DIR)/clientmode_hl2mpnormal.o : $(PWD)/hl2mp/clientmode_hl2mpnormal.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_hl2mp_player.P
endif

$(OBJ_DIR)/c_hl2mp_player.o : $(PWD)/hl2mp/c_hl2mp_player.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/c_te_hl2mp_shotgun_shot.P
endif

$(OBJ_DIR)/c_te_hl2mp_shotgun_shot.o : $(PWD)/hl2mp/c_te_hl2mp_shotgun_shot.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hl2mp_hud_chat.P
endif

$(OBJ_DIR)/hl2mp_hud_chat.o : $(PWD)/hl2mp/hl2mp_hud_chat.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hl2mp_hud_target_id.P
endif

$(OBJ_DIR)/hl2mp_hud_target_id.o : $(PWD)/hl2mp/hl2mp_hud_target_id.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hl2mp_hud_team.P
endif

$(OBJ_DIR)/hl2mp_hud_team.o : $(PWD)/hl2mp/hl2mp_hud_team.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_deathnotice.P
endif

$(OBJ_DIR)/hud_deathnotice.o : $(PWD)/hl2mp/hud_deathnotice.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/backgroundpanel.P
endif

$(OBJ_DIR)/backgroundpanel.o : $(PWD)/hl2mp/ui/backgroundpanel.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hl2mpclientscoreboard.P
endif

$(OBJ_DIR)/hl2mpclientscoreboard.o : $(PWD)/hl2mp/ui/hl2mpclientscoreboard.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hl2mptextwindow.P
endif

$(OBJ_DIR)/hl2mptextwindow.o : $(PWD)/hl2mp/ui/hl2mptextwindow.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hltvcamera.P
endif

$(OBJ_DIR)/hltvcamera.o : $(PWD)/hltvcamera.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud.P
endif

$(OBJ_DIR)/hud.o : $(PWD)/hud.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_animationinfo.P
endif

$(OBJ_DIR)/hud_animationinfo.o : $(PWD)/hud_animationinfo.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_basechat.P
endif

$(OBJ_DIR)/hud_basechat.o : $(PWD)/hud_basechat.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_basetimer.P
endif

$(OBJ_DIR)/hud_basetimer.o : $(PWD)/hud_basetimer.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_bitmapnumericdisplay.P
endif

$(OBJ_DIR)/hud_bitmapnumericdisplay.o : $(PWD)/hud_bitmapnumericdisplay.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_closecaption.P
endif

$(OBJ_DIR)/hud_closecaption.o : $(PWD)/hud_closecaption.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_crosshair.P
endif

$(OBJ_DIR)/hud_crosshair.o : $(PWD)/hud_crosshair.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_element_helper.P
endif

$(OBJ_DIR)/hud_element_helper.o : $(PWD)/hud_element_helper.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_hintdisplay.P
endif

$(OBJ_DIR)/hud_hintdisplay.o : $(PWD)/hud_hintdisplay.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_lcd.P
endif

$(OBJ_DIR)/hud_lcd.o : $(PWD)/hud_lcd.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_msg.P
endif

$(OBJ_DIR)/hud_msg.o : $(PWD)/hud_msg.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_numericdisplay.P
endif

$(OBJ_DIR)/hud_numericdisplay.o : $(PWD)/hud_numericdisplay.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_pdump.P
endif

$(OBJ_DIR)/hud_pdump.o : $(PWD)/hud_pdump.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_redraw.P
endif

$(OBJ_DIR)/hud_redraw.o : $(PWD)/hud_redraw.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_squadstatus.P
endif

$(OBJ_DIR)/hud_squadstatus.o : $(PWD)/hud_squadstatus.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_vehicle.P
endif

$(OBJ_DIR)/hud_vehicle.o : $(PWD)/hud_vehicle.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_voicestatus.P
endif

$(OBJ_DIR)/hud_voicestatus.o : $(PWD)/hud_voicestatus.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/hud_weapon.P
endif

$(OBJ_DIR)/hud_weapon.o : $(PWD)/hud_weapon.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/initializer.P
endif

$(OBJ_DIR)/initializer.o : $(PWD)/initializer.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/interpolatedvar.P
endif

$(OBJ_DIR)/interpolatedvar.o : $(PWD)/interpolatedvar.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/in_camera.P
endif

$(OBJ_DIR)/in_camera.o : $(PWD)/in_camera.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/in_joystick.P
endif

$(OBJ_DIR)/in_joystick.o : $(PWD)/in_joystick.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/in_main.P
endif

$(OBJ_DIR)/in_main.o : $(PWD)/in_main.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/in_mouse.P
endif

$(OBJ_DIR)/in_mouse.o : $(PWD)/in_mouse.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/IsNPCProxy.P
endif

$(OBJ_DIR)/IsNPCProxy.o : $(PWD)/IsNPCProxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/lampbeamproxy.P
endif

$(OBJ_DIR)/lampbeamproxy.o : $(PWD)/lampbeamproxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/lamphaloproxy.P
endif

$(OBJ_DIR)/lamphaloproxy.o : $(PWD)/lamphaloproxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/mathproxy.P
endif

$(OBJ_DIR)/mathproxy.o : $(PWD)/mathproxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/matrixproxy.P
endif

$(OBJ_DIR)/matrixproxy.o : $(PWD)/matrixproxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/menu.P
endif

$(OBJ_DIR)/menu.o : $(PWD)/menu.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/message.P
endif

$(OBJ_DIR)/message.o : $(PWD)/message.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/movehelper_client.P
endif

$(OBJ_DIR)/movehelper_client.o : $(PWD)/movehelper_client.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/mp3player.P
endif

$(OBJ_DIR)/mp3player.o : $(PWD)/mp3player.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/mumble.P
endif

$(OBJ_DIR)/mumble.o : $(PWD)/mumble.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/panelmetaclassmgr.P
endif

$(OBJ_DIR)/panelmetaclassmgr.o : $(PWD)/panelmetaclassmgr.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/particlemgr.P
endif

$(OBJ_DIR)/particlemgr.o : $(PWD)/particlemgr.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/particlesphererenderer.P
endif

$(OBJ_DIR)/particlesphererenderer.o : $(PWD)/particlesphererenderer.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/particles_attractor.P
endif

$(OBJ_DIR)/particles_attractor.o : $(PWD)/particles_attractor.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/particles_ez.P
endif

$(OBJ_DIR)/particles_ez.o : $(PWD)/particles_ez.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/particles_localspace.P
endif

$(OBJ_DIR)/particles_localspace.o : $(PWD)/particles_localspace.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/particles_new.P
endif

$(OBJ_DIR)/particles_new.o : $(PWD)/particles_new.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/particles_simple.P
endif

$(OBJ_DIR)/particles_simple.o : $(PWD)/particles_simple.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/particle_collision.P
endif

$(OBJ_DIR)/particle_collision.o : $(PWD)/particle_collision.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/particle_litsmokeemitter.P
endif

$(OBJ_DIR)/particle_litsmokeemitter.o : $(PWD)/particle_litsmokeemitter.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/particle_proxies.P
endif

$(OBJ_DIR)/particle_proxies.o : $(PWD)/particle_proxies.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/particle_simple3d.P
endif

$(OBJ_DIR)/particle_simple3d.o : $(PWD)/particle_simple3d.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/perfvisualbenchmark.P
endif

$(OBJ_DIR)/perfvisualbenchmark.o : $(PWD)/perfvisualbenchmark.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/physics.P
endif

$(OBJ_DIR)/physics.o : $(PWD)/physics.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/physics_main_client.P
endif

$(OBJ_DIR)/physics_main_client.o : $(PWD)/physics_main_client.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/physpropclientside.P
endif

$(OBJ_DIR)/physpropclientside.o : $(PWD)/physpropclientside.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/playerandobjectenumerator.P
endif

$(OBJ_DIR)/playerandobjectenumerator.o : $(PWD)/playerandobjectenumerator.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/playerspawncache.P
endif

$(OBJ_DIR)/playerspawncache.o : $(PWD)/playerspawncache.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/prediction.P
endif

$(OBJ_DIR)/prediction.o : $(PWD)/prediction.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/proxyentity.P
endif

$(OBJ_DIR)/proxyentity.o : $(PWD)/proxyentity.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ProxyHealth.P
endif

$(OBJ_DIR)/ProxyHealth.o : $(PWD)/ProxyHealth.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/proxyplayer.P
endif

$(OBJ_DIR)/proxyplayer.o : $(PWD)/proxyplayer.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/proxypupil.P
endif

$(OBJ_DIR)/proxypupil.o : $(PWD)/proxypupil.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ragdoll.P
endif

$(OBJ_DIR)/ragdoll.o : $(PWD)/ragdoll.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/recvproxy.P
endif

$(OBJ_DIR)/recvproxy.o : $(PWD)/recvproxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/rendertexture.P
endif

$(OBJ_DIR)/rendertexture.o : $(PWD)/rendertexture.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/cdll_replay.P
endif

$(OBJ_DIR)/cdll_replay.o : $(PWD)/replay/cdll_replay.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/replaycamera.P
endif

$(OBJ_DIR)/replaycamera.o : $(PWD)/replay/replaycamera.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ScreenSpaceEffects.P
endif

$(OBJ_DIR)/ScreenSpaceEffects.o : $(PWD)/ScreenSpaceEffects.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/simple_keys.P
endif

$(OBJ_DIR)/simple_keys.o : $(PWD)/simple_keys.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/in_sixense.P
endif

$(OBJ_DIR)/in_sixense.o : $(PWD)/sixense/in_sixense.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/in_sixense_gesture_bindings.P
endif

$(OBJ_DIR)/in_sixense_gesture_bindings.o : $(PWD)/sixense/in_sixense_gesture_bindings.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/smoke_fog_overlay.P
endif

$(OBJ_DIR)/smoke_fog_overlay.o : $(PWD)/smoke_fog_overlay.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/splinepatch.P
endif

$(OBJ_DIR)/splinepatch.o : $(PWD)/splinepatch.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/spritemodel.P
endif

$(OBJ_DIR)/spritemodel.o : $(PWD)/spritemodel.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/stdafx.P
endif

$(OBJ_DIR)/stdafx.o : $(PWD)/stdafx.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/studio_stats.P
endif

$(OBJ_DIR)/studio_stats.o : $(PWD)/studio_stats.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/texturescrollmaterialproxy.P
endif

$(OBJ_DIR)/texturescrollmaterialproxy.o : $(PWD)/texturescrollmaterialproxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/text_message.P
endif

$(OBJ_DIR)/text_message.o : $(PWD)/text_message.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/timematerialproxy.P
endif

$(OBJ_DIR)/timematerialproxy.o : $(PWD)/timematerialproxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/toggletextureproxy.P
endif

$(OBJ_DIR)/toggletextureproxy.o : $(PWD)/toggletextureproxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/toolframework_client.P
endif

$(OBJ_DIR)/toolframework_client.o : $(PWD)/toolframework_client.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/train.P
endif

$(OBJ_DIR)/train.o : $(PWD)/train.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_avatarimage.P
endif

$(OBJ_DIR)/vgui_avatarimage.o : $(PWD)/vgui_avatarimage.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_basepanel.P
endif

$(OBJ_DIR)/vgui_basepanel.o : $(PWD)/vgui_basepanel.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_bitmapbutton.P
endif

$(OBJ_DIR)/vgui_bitmapbutton.o : $(PWD)/vgui_bitmapbutton.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_bitmapimage.P
endif

$(OBJ_DIR)/vgui_bitmapimage.o : $(PWD)/vgui_bitmapimage.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_bitmappanel.P
endif

$(OBJ_DIR)/vgui_bitmappanel.o : $(PWD)/vgui_bitmappanel.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_centerstringpanel.P
endif

$(OBJ_DIR)/vgui_centerstringpanel.o : $(PWD)/vgui_centerstringpanel.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_consolepanel.P
endif

$(OBJ_DIR)/vgui_consolepanel.o : $(PWD)/vgui_consolepanel.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_debugoverlaypanel.P
endif

$(OBJ_DIR)/vgui_debugoverlaypanel.o : $(PWD)/vgui_debugoverlaypanel.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_fpspanel.P
endif

$(OBJ_DIR)/vgui_fpspanel.o : $(PWD)/vgui_fpspanel.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_game_viewport.P
endif

$(OBJ_DIR)/vgui_game_viewport.o : $(PWD)/vgui_game_viewport.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_grid.P
endif

$(OBJ_DIR)/vgui_grid.o : $(PWD)/vgui_grid.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_int.P
endif

$(OBJ_DIR)/vgui_int.o : $(PWD)/vgui_int.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_loadingdiscpanel.P
endif

$(OBJ_DIR)/vgui_loadingdiscpanel.o : $(PWD)/vgui_loadingdiscpanel.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_messagechars.P
endif

$(OBJ_DIR)/vgui_messagechars.o : $(PWD)/vgui_messagechars.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_netgraphpanel.P
endif

$(OBJ_DIR)/vgui_netgraphpanel.o : $(PWD)/vgui_netgraphpanel.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_schemevisualizer.P
endif

$(OBJ_DIR)/vgui_schemevisualizer.o : $(PWD)/vgui_schemevisualizer.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_slideshow_display_screen.P
endif

$(OBJ_DIR)/vgui_slideshow_display_screen.o : $(PWD)/vgui_slideshow_display_screen.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_video.P
endif

$(OBJ_DIR)/vgui_video.o : $(PWD)/vgui_video.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_video_player.P
endif

$(OBJ_DIR)/vgui_video_player.o : $(PWD)/vgui_video_player.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/view.P
endif

$(OBJ_DIR)/view.o : $(PWD)/view.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/viewangleanim.P
endif

$(OBJ_DIR)/viewangleanim.o : $(PWD)/viewangleanim.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ViewConeImage.P
endif

$(OBJ_DIR)/ViewConeImage.o : $(PWD)/ViewConeImage.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/viewdebug.P
endif

$(OBJ_DIR)/viewdebug.o : $(PWD)/viewdebug.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/viewpostprocess.P
endif

$(OBJ_DIR)/viewpostprocess.o : $(PWD)/viewpostprocess.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/viewrender.P
endif

$(OBJ_DIR)/viewrender.o : $(PWD)/viewrender.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/view_beams.P
endif

$(OBJ_DIR)/view_beams.o : $(PWD)/view_beams.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/view_effects.P
endif

$(OBJ_DIR)/view_effects.o : $(PWD)/view_effects.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/view_scene.P
endif

$(OBJ_DIR)/view_scene.o : $(PWD)/view_scene.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/warp_overlay.P
endif

$(OBJ_DIR)/warp_overlay.o : $(PWD)/warp_overlay.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/WaterLODMaterialProxy.P
endif

$(OBJ_DIR)/WaterLODMaterialProxy.o : $(PWD)/WaterLODMaterialProxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/weapons_resource.P
endif

$(OBJ_DIR)/weapons_resource.o : $(PWD)/weapons_resource.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/weapon_selection.P
endif

$(OBJ_DIR)/weapon_selection.o : $(PWD)/weapon_selection.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/WorldDimsProxy.P
endif

$(OBJ_DIR)/WorldDimsProxy.o : $(PWD)/WorldDimsProxy.cpp $(PWD)/client_linux32_hl2mp.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

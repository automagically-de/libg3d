#ifndef _IMP_ACF_DEF_ACF740_H
#define _IMP_ACF_DEF_ACF740_H

#include "imp_acf_def_proto.h"

static const AcfDef acf_def_acf740[] = {
	{ XCHR, "HEADER_platform",                          1 },
	{ XINT, "HEADER_version",                           1 },
	{ XFLT, "HEADER_filler",                            1 },
	{ XCHR, "VIEW_name",                              500 },
	{ XCHR, "VIEW_path",                              500 },
	{ XCHR, "VIEW_tailnum",                            40 },
	{ XCHR, "VIEW_author",                            500 },
	{ XCHR, "VIEW_descrip",                           500 },
	{ XFLT, "VIEW_Vmca_kts",                            1 },
	{ XFLT, "VIEW_Vso_kts",                             1 },
	{ XFLT, "VIEW_Vs_kts",                              1 },
	{ XFLT, "VIEW_Vyse_kts",                            1 },
	{ XFLT, "VIEW_Vfe_kts",                             1 },
	{ XFLT, "VIEW_Vle_kts",                             1 },
	{ XFLT, "VIEW_Vno_kts",                             1 },
	{ XFLT, "VIEW_Vne_kts",                             1 },
	{ XFLT, "VIEW_Mmo",                                 1 },
	{ XFLT, "VIEW_Gneg",                                1 },
	{ XFLT, "VIEW_Gpos",                                1 },
	{ XINT, "VIEW_has_navlites",                        1 },
	{ XFLT, "VIEW_pe_xyz",                              3 },
	{ XINT, "VIEW_has_lanlite1",                        1 },
	{ XFLT, "VIEW_lanlite1_xyz",                        3 },
	{ XINT, "VIEW_has_lanlite2",                        1 },
	{ XFLT, "VIEW_lanlite2_xyz",                        3 },
	{ XINT, "VIEW_has_taxilite",                        1 },
	{ XFLT, "VIEW_lanlite3_xyz",                        3 },
	{ XINT, "VIEW_has_fuserb1",                         1 },
	{ XFLT, "VIEW_fuserb1_xyz",                         3 },
	{ XINT, "VIEW_has_fuserb2",                         1 },
	{ XFLT, "VIEW_fuserb2_xyz",                         3 },
	{ XINT, "VIEW_has_taillite",                        1 },
	{ XFLT, "VIEW_taillite_xyz",                        3 },
	{ XINT, "VIEW_has_refuel",                          1 },
	{ XFLT, "VIEW_refuel_xyz",                          3 },
	{ XFLT, "VIEW_yawstring_x",                         1 },
	{ XFLT, "VIEW_yawstring_y",                         1 },
	{ XFLT, "VIEW_HUD_ctr_x",                           1 },
	{ XFLT, "VIEW_HUD_ctr_y_OLD",                       1 },
	{ XFLT, "VIEW_HUD_del_x",                           1 },
	{ XFLT, "VIEW_HUD_del_y",                           1 },
	{ XINT, "VIEW_lan_lite_steers",                     1 },
	{ XFLT, "VIEW_lan_lite_power",                      1 },
	{ XFLT, "VIEW_lan_lite_width",                      1 },
	{ XFLT, "VIEW_lan_lite_the_ref",                    1 },
	{ XFLT, "VIEW_stall_warn_aoa",                      1 },
	{ XFLT, "VIEW_tow_hook_Y",                          1 },
	{ XFLT, "VIEW_tow_hook_Z",                          1 },
	{ XFLT, "VIEW_win_hook_Y",                          1 },
	{ XFLT, "VIEW_win_hook_Z",                          1 },
	{ XINT, "VIEW_has_HOOPS_HUD",                       1 },
	{ XINT, "VIEW_cockpit_type",                        1 },
	{ XINT, "VIEW_asi_is_kts",                          1 },
	{ XINT, "VIEW_warn1_EQ",                            1 },
	{ XINT, "VIEW_warn2_EQ",                            1 },
	{ XINT, "VIEW_is_glossy",                           1 },
	{ XINT, "VIEW_draw_geo_frnt_views",                 1 },
	{ XINT, "VIEW_draw_geo_side_views",                 1 },
	{ XINT, "VIEW_ins_type",                          300 },
	{ XFLT, "VIEW_ins_size",                          300 },
	{ XFLT, "VIEW_ins_x",                             300 },
	{ XFLT, "VIEW_ins_y",                             300 },
	{ XINT, "VIEW_cus_rnd_use",                        50 },
	{ XFLT, "VIEW_cus_rnd_lo_val",                     50 },
	{ XFLT, "VIEW_cus_rnd_hi_val",                     50 },
	{ XFLT, "VIEW_cus_rnd_lo_ang",                     50 },
	{ XFLT, "VIEW_cus_rnd_hi_ang",                     50 },
	{ XINT, "VIEW_cus_rnd_mirror",                     50 },
	{ XINT, "VIEW_cus_rnd_label",                      50 },
	{ XINT, "VIEW_cus_dig_use",                        50 },
	{ XFLT, "VIEW_cus_dig_offset",                     50 },
	{ XFLT, "VIEW_cus_dig_scale",                      50 },
	{ XINT, "VIEW_cus_dig_dig",                        50 },
	{ XINT, "VIEW_cus_dig_dec",                        50 },
	{ XINT, "ENGINE_num_engines",                       1 },
	{ XINT, "ENGINE_num_thrustpoints",                  1 },
	{ XFLT, "ENGINE_throt_max_FWD",                     1 },
	{ XFLT, "ENGINE_throt_max_REV",                     1 },
	{ XFLT, "ENGINE_idle_rat",                          2 },
	{ XINT, "ENGINE_props_linked",                      1 },
	{ XINT, "ENGINE_beta_prop_EQ",                      1 },
	{ XINT, "ENGINE_auto_feather_EQ",                   1 },
	{ XINT, "ENGINE_rev_thrust_EQ",                     1 },
	{ XINT, "ENGINE_drive_by_wire_EQ",                  1 },
	{ XFLT, "ENGINE_feathered_pitch",                   1 },
	{ XFLT, "ENGINE_reversed_pitch",                    1 },
	{ XFLT, "ENGINE_rotor_mi_rat",                      1 },
	{ XFLT, "ENGINE_tip_weight",                        1 },
	{ XFLT, "ENGINE_tip_mach_des_100",                  1 },
	{ XFLT, "ENGINE_tip_mach_des_50",                   1 },
	{ XFLT, "ENGINE_power_max",                         1 },
	{ XFLT, "ENGINE_crit_alt",                          1 },
	{ XFLT, "ENGINE_MP_max",                            1 },
	{ XFLT, "ENGINE_trq_max_eng",                       1 },
	{ XFLT, "ENGINE_RSC_idlespeed_ENGN",                1 },
	{ XFLT, "ENGINE_RSC_redline_ENGN",                  1 },
	{ XFLT, "ENGINE_RSC_idlespeed_PROP",                1 },
	{ XFLT, "ENGINE_RSC_redline_PROP",                  1 },
	{ XFLT, "ENGINE_RSC_mingreen_ENGN",                 1 },
	{ XFLT, "ENGINE_RSC_maxgreen_ENGN",                 1 },
	{ XFLT, "ENGINE_RSC_mingreen_PROP",                 1 },
	{ XFLT, "ENGINE_RSC_maxgreen_PROP",                 1 },
	{ XINT, "ENGINE_has_press_controls",                1 },
	{ XFLT, "ENGINE_throt_time_prop",                   1 },
	{ XFLT, "ENGINE_trans_loss",                        1 },
	{ XFLT, "ENGINE_thrust_max",                        1 },
	{ XFLT, "ENGINE_burner_inc",                        1 },
	{ XFLT, "ENGINE_max_mach_eff",                      1 },
	{ XFLT, "ENGINE_face_jet",                          1 },
	{ XFLT, "ENGINE_throt_time_jet",                    1 },
	{ XFLT, "ENGINE_lift_fan_rat",                      1 },
	{ XFLT, "ENGINE_rock_max_sl",                       1 },
	{ XFLT, "ENGINE_rock_max_opt",                      1 },
	{ XFLT, "ENGINE_rock_max_vac",                      1 },
	{ XFLT, "ENGINE_rock_h_opt",                        1 },
	{ XFLT, "ENGINE_face_rocket",                       1 },
	{ XINT, "PROP_engn_type",                           8 },
	{ XINT, "PROP_prop_type",                           8 },
	{ XFLT, "PROP_engn_mass",                           8 },
	{ XINT, "PROP_prop_clutch_EQ",                      8 },
	{ XFLT, "PROP_prop_gear_rat",                       8 },
	{ XFLT, "PROP_prop_dir",                            8 },
	{ XFLT, "PROP_num_blades",                          8 },
	{ XFLT, "PROP_SFC",                                 8 },
	{ XFLT, "PROP_vert_cant_init",                      8 },
	{ XFLT, "PROP_side_cant_init",                      8 },
	{ XFLT, "PROP_min_pitch",                           8 },
	{ XFLT, "PROP_max_pitch",                           8 },
	{ XFLT, "PROP_des_rpm_prp",                         8 },
	{ XFLT, "PROP_des_kts_prp",                         8 },
	{ XFLT, "PROP_des_kts_acf",                         8 },
	{ XFLT, "PROP_prop_mass",                           8 },
	{ XFLT, "PROP_mi_prop_rpm",                         8 },
	{ XFLT, "PROP_mi_engn_rpm",                         8 },
	{ XFLT, "PROP_discarea",                            8 },
	{ XFLT, "PROP_ringarea",                           80 },
	{ XFLT, "PROP_bladesweep",                         80 },
	{ XFLT, "HYDRO_STICK_starter_rat",                  1 },
	{ XFLT, "HYDRO_STICK_battery_rat",                  1 },
	{ XINT, "HYDRO_STICK_ff_hydraulic",                 1 },
	{ XINT, "HYDRO_STICK_ff_stickshaker",               1 },
	{ XFLT, "HYDRO_STICK_manual_reversion_rat",         1 },
	{ XFLT, "HYDRO_STICK_max_press_diff",               1 },
	{ XINT, "PARTS_part_eq",                           73 },
	{ XCHR, "PARTS_Rafl0",                           2920 },
	{ XCHR, "PARTS_Rafl1",                           2920 },
	{ XCHR, "PARTS_Tafl0",                           2920 },
	{ XCHR, "PARTS_Tafl1",                           2920 },
	{ XINT, "PARTS_els",                               73 },
	{ XFLT, "PARTS_Xarm",                              73 },
	{ XFLT, "PARTS_Yarm",                              73 },
	{ XFLT, "PARTS_Zarm",                              73 },
	{ XFLT, "PARTS_Croot",                             73 },
	{ XFLT, "PARTS_Ctip",                              73 },
	{ XFLT, "PARTS_semilen_SEG",                       73 },
	{ XFLT, "PARTS_semilen_JND",                       73 },
	{ XFLT, "PARTS_element_len",                       73 },
	{ XFLT, "PARTS_X_body_aero",                       73 },
	{ XFLT, "PARTS_Y_body_aero",                       73 },
	{ XFLT, "PARTS_Z_body_aero",                       73 },
	{ XFLT, "PARTS_dihed1",                            73 },
	{ XFLT, "PARTS_dihed2",                            73 },
	{ XFLT, "PARTS_dihednow",                          73 },
	{ XINT, "PARTS_vardihed",                          73 },
	{ XINT, "PARTS_vardihedEQ",                         1 },
	{ XFLT, "PARTS_sweep1",                            73 },
	{ XFLT, "PARTS_sweep2",                            73 },
	{ XFLT, "PARTS_sweepnow",                          73 },
	{ XINT, "PARTS_varsweep",                          73 },
	{ XINT, "PARTS_varsweepEQ",                         1 },
	{ XFLT, "PARTS_e",                                 73 },
	{ XFLT, "PARTS_AR",                                73 },
	{ XFLT, "PARTS_al_D_al0",                          73 },
	{ XFLT, "PARTS_cl_D_cl0",                          73 },
	{ XFLT, "PARTS_cm_D_cm0",                          73 },
	{ XFLT, "PARTS_delta_fac",                         73 },
	{ XFLT, "PARTS_spec_wash",                         73 },
	{ XFLT, "PARTS_alpha_max",                         73 },
	{ XFLT, "PARTS_slat_effect",                       73 },
	{ XFLT, "PARTS_s",                                730 },
	{ XFLT, "PARTS_mac",                              730 },
	{ XFLT, "PARTS_incidence",                        730 },
	{ XINT, "PARTS_ail1",                             730 },
	{ XFLT, "PARTS_ail1_elR",                          73 },
	{ XFLT, "PARTS_ail1_elT",                          73 },
	{ XFLT, "PARTS_ail1_cratR",                         1 },
	{ XFLT, "PARTS_ail1_cratT",                         1 },
	{ XFLT, "PARTS_ail1_up",                            1 },
	{ XFLT, "PARTS_ail1_dn",                            1 },
	{ XINT, "PARTS_ail2",                             730 },
	{ XFLT, "PARTS_ail2_elR",                          73 },
	{ XFLT, "PARTS_ail2_elT",                          73 },
	{ XFLT, "PARTS_ail2_cratR",                         1 },
	{ XFLT, "PARTS_ail2_cratT",                         1 },
	{ XFLT, "PARTS_ail2_up",                            1 },
	{ XFLT, "PARTS_ail2_dn",                            1 },
	{ XINT, "PARTS_elev",                             730 },
	{ XFLT, "PARTS_elev_elR",                          73 },
	{ XFLT, "PARTS_elev_elT",                          73 },
	{ XFLT, "PARTS_elev_cratR",                         1 },
	{ XFLT, "PARTS_elev_cratT",                         1 },
	{ XFLT, "PARTS_elev_up",                            1 },
	{ XFLT, "PARTS_elev_dn",                            1 },
	{ XINT, "PARTS_rudd",                             730 },
	{ XFLT, "PARTS_rudd_elR",                          73 },
	{ XFLT, "PARTS_rudd_elT",                          73 },
	{ XFLT, "PARTS_rudd_cratR",                         1 },
	{ XFLT, "PARTS_rudd_cratT",                         1 },
	{ XFLT, "PARTS_rudd_lr",                            1 },
	{ XINT, "PARTS_spo1",                             730 },
	{ XFLT, "PARTS_spo1_elR",                          73 },
	{ XFLT, "PARTS_spo1_elT",                          73 },
	{ XFLT, "PARTS_spo1_cratR",                         1 },
	{ XFLT, "PARTS_spo1_cratT",                         1 },
	{ XFLT, "PARTS_spo1_up",                            1 },
	{ XINT, "PARTS_yawb",                             730 },
	{ XFLT, "PARTS_yawb_elR",                          73 },
	{ XFLT, "PARTS_yawb_elT",                          73 },
	{ XFLT, "PARTS_yawb_cratR",                         1 },
	{ XFLT, "PARTS_yawb_cratT",                         1 },
	{ XFLT, "PARTS_yawb_ud",                            1 },
	{ XINT, "PARTS_sbrk",                             730 },
	{ XFLT, "PARTS_sbrk_elR",                          73 },
	{ XFLT, "PARTS_sbrk_elT",                          73 },
	{ XFLT, "PARTS_sbrk_cratR",                         1 },
	{ XFLT, "PARTS_sbrk_cratT",                         1 },
	{ XFLT, "PARTS_sbrk_up",                            1 },
	{ XINT, "PARTS_sbrkEQ",                             1 },
	{ XINT, "PARTS_flap",                             730 },
	{ XFLT, "PARTS_flap_elR",                          73 },
	{ XFLT, "PARTS_flap_elT",                          73 },
	{ XFLT, "PARTS_flap_cratR",                         1 },
	{ XFLT, "PARTS_flap_cratT",                         1 },
	{ XFLT, "PARTS_flap_dn",                            8 },
	{ XINT, "PARTS_flapEQ",                             1 },
	{ XINT, "PARTS_slat",                             730 },
	{ XFLT, "PARTS_slat_inc",                           1 },
	{ XINT, "PARTS_slatEQ",                             1 },
	{ XINT, "PARTS_inc_ail1",                         730 },
	{ XINT, "PARTS_inc_ail2",                         730 },
	{ XINT, "PARTS_inc_elev",                         730 },
	{ XINT, "PARTS_inc_rudd",                         730 },
	{ XINT, "PARTS_inc_vect",                         730 },
	{ XINT, "PARTS_inc_trim",                         730 },
	{ XINT, "PARTS_in_downwash",                     53290 },
	{ XFLT, "PARTS_body_r",                            73 },
	{ XFLT, "PARTS_body_X",                          26280 },
	{ XFLT, "PARTS_body_Y",                          26280 },
	{ XFLT, "PARTS_body_Z",                          26280 },
	{ XINT, "PARTS_gear_type",                         73 },
	{ XFLT, "PARTS_gear_latE",                         73 },
	{ XFLT, "PARTS_gear_lonE",                         73 },
	{ XFLT, "PARTS_gear_axiE",                         73 },
	{ XFLT, "PARTS_gear_latR",                         73 },
	{ XFLT, "PARTS_gear_lonR",                         73 },
	{ XFLT, "PARTS_gear_axiR",                         73 },
	{ XFLT, "PARTS_gear_latN",                         73 },
	{ XFLT, "PARTS_gear_lonN",                         73 },
	{ XFLT, "PARTS_gear_axiN",                         73 },
	{ XFLT, "PARTS_gear_xnodef",                       73 },
	{ XFLT, "PARTS_gear_ynodef",                       73 },
	{ XFLT, "PARTS_gear_znodef",                       73 },
	{ XFLT, "PARTS_gear_leglen",                       73 },
	{ XFLT, "PARTS_tire_radius",                       73 },
	{ XFLT, "PARTS_tire_swidth",                       73 },
	{ XFLT, "PARTS_gearcon",                           73 },
	{ XFLT, "PARTS_geardmp",                           73 },
	{ XFLT, "PARTS_gear_deploy",                       73 },
	{ XFLT, "PARTS_gearstatdef",                       73 },
	{ XFLT, "PARTS_dummy",                             73 },
	{ XINT, "PARTS_gear_steers",                       73 },
	{ XFLT, "PARTS_gear_cyctim",                       73 },
	{ XFLT, "BODIES_fuse_cd",                           1 },
	{ XFLT, "CONTROL_hstb_trim_up",                     1 },
	{ XFLT, "CONTROL_hstb_trim_dn",                     1 },
	{ XINT, "CONTROL_flap_type",                        1 },
	{ XINT, "CONTROL_con_smooth",                       1 },
	{ XINT, "CONTROL_flap_detents",                     1 },
	{ XFLT, "CONTROL_flap_deftime",                     1 },
	{ XFLT, "CONTROL_flap_cl",                          1 },
	{ XFLT, "CONTROL_flap_cd",                          1 },
	{ XFLT, "CONTROL_flap_cm",                          1 },
	{ XFLT, "CONTROL_blown_flap_add_speed",             1 },
	{ XFLT, "CONTROL_blown_flap_throt_red",             1 },
	{ XFLT, "CONTROL_blown_flap_min_engag",             1 },
	{ XINT, "CONTROL_blow_all_controls",                1 },
	{ XINT, "LANDING_GEAR_gear_retract",                1 },
	{ XFLT, "LANDING_GEAR_nw_steerdeg1",                1 },
	{ XFLT, "LANDING_GEAR_nw_steerdeg2",                1 },
	{ XFLT, "LANDING_GEAR_nw_cutoff_omega",             1 },
	{ XFLT, "LANDING_GEAR_nw_side_k",                   1 },
	{ XFLT, "LANDING_GEAR_gear_door_size",              1 },
	{ XFLT, "LANDING_GEAR_water_rud_longarm",           1 },
	{ XFLT, "LANDING_GEAR_water_rud_area",              1 },
	{ XFLT, "LANDING_GEAR_water_rud_maxdef",            1 },
	{ XFLT, "LANDING_GEAR_roll_co",                     1 },
	{ XFLT, "LANDING_GEAR_brake_co",                    1 },
	{ XINT, "LANDING_GEAR_gear_door_typ",              10 },
	{ XFLT, "LANDING_GEAR_gear_door_loc",              30 },
	{ XFLT, "LANDING_GEAR_gear_door_geo",             120 },
	{ XFLT, "LANDING_GEAR_gear_door_axi_rot",          10 },
	{ XFLT, "LANDING_GEAR_gear_door_ext_ang",          10 },
	{ XFLT, "LANDING_GEAR_gear_door_ret_ang",          10 },
	{ XFLT, "LANDING_GEAR_gear_door_ang_now",          10 },
	{ XFLT, "WEIGHT_BALANCE_cgY",                       1 },
	{ XFLT, "WEIGHT_BALANCE_cgZ",                       1 },
	{ XFLT, "WEIGHT_BALANCE_cgZ_fwd",                   1 },
	{ XFLT, "WEIGHT_BALANCE_cgZ_aft",                   1 },
	{ XFLT, "WEIGHT_BALANCE_m_empty",                   1 },
	{ XFLT, "WEIGHT_BALANCE_m_fuel_tot",                1 },
	{ XFLT, "WEIGHT_BALANCE_m_jettison",                1 },
	{ XFLT, "WEIGHT_BALANCE_m_max",                     1 },
	{ XFLT, "WEIGHT_BALANCE_m_displaced",               1 },
	{ XFLT, "WEIGHT_BALANCE_Jxx_unitmass",              1 },
	{ XFLT, "WEIGHT_BALANCE_Jyy_unitmass",              1 },
	{ XFLT, "WEIGHT_BALANCE_Jzz_unitmass",              1 },
	{ XINT, "WEIGHT_BALANCE_num_tanks",                 1 },
	{ XFLT, "WEIGHT_BALANCE_tank_rat",                  3 },
	{ XFLT, "WEIGHT_BALANCE_tank_X",                    3 },
	{ XFLT, "WEIGHT_BALANCE_tank_Y",                    3 },
	{ XFLT, "WEIGHT_BALANCE_tank_Z",                    3 },
	{ XINT, "WEIGHT_BALANCE_jett_is_slung",             1 },
	{ XINT, "WEIGHT_BALANCE_jett_is_water",             1 },
	{ XFLT, "WEIGHT_BALANCE_jett_len",                  1 },
	{ XFLT, "WEIGHT_BALANCE_jett_xyz",                  3 },
	{ XFLT, "SPECIAL_CONTROLS_flap_roll",               1 },
	{ XFLT, "SPECIAL_CONTROLS_flap_ptch",               1 },
	{ XFLT, "SPECIAL_CONTROLS_mass_shift",              1 },
	{ XFLT, "SPECIAL_CONTROLS_mass_shift_dx",           1 },
	{ XFLT, "SPECIAL_CONTROLS_mass_shift_dz",           1 },
	{ XFLT, "SPECIAL_CONTROLS_wing_tilt_ptch",          1 },
	{ XFLT, "SPECIAL_CONTROLS_wing_tilt_roll",          1 },
	{ XFLT, "SPECIAL_CONTROLS_tvec_ptch",               1 },
	{ XFLT, "SPECIAL_CONTROLS_tvec_roll",               1 },
	{ XFLT, "SPECIAL_CONTROLS_tvec_hdng",               1 },
	{ XFLT, "SPECIAL_CONTROLS_jato_Y",                  1 },
	{ XFLT, "SPECIAL_CONTROLS_jato_Z",                  1 },
	{ XFLT, "SPECIAL_CONTROLS_jato_theta",              1 },
	{ XFLT, "SPECIAL_CONTROLS_jato_thrust",             1 },
	{ XFLT, "SPECIAL_CONTROLS_jato_dur",                1 },
	{ XFLT, "SPECIAL_CONTROLS_jato_sfc",                1 },
	{ XFLT, "SPECIAL_CONTROLS_stab_roll",               1 },
	{ XFLT, "SPECIAL_CONTROLS_stab_hdng",               1 },
	{ XFLT, "SPECIAL_CONTROLS_elev_with_flap",          1 },
	{ XFLT, "SPECIAL_CONTROLS_ail1_pitch",              1 },
	{ XFLT, "SPECIAL_CONTROLS_ail1_flaps",              1 },
	{ XFLT, "SPECIAL_CONTROLS_ail2_pitch",              1 },
	{ XFLT, "SPECIAL_CONTROLS_ail2_flaps",              1 },
	{ XFLT, "SPECIAL_CONTROLS_ail2_vmax",               1 },
	{ XFLT, "SPECIAL_CONTROLS_diff_thro_hdng",          1 },
	{ XINT, "SPECIAL_CONTROLS_phase_ptch_tvect_in_at_90",    1 },
	{ XINT, "SPECIAL_CONTROLS_phase_ptch_tvect_in_at_00",    1 },
	{ XINT, "SPECIAL_CONTROLS_sbrk_on_td_EQ",           1 },
	{ XINT, "SPECIAL_CONTROLS_fbrk_on_td_EQ",           1 },
	{ XINT, "SPECIAL_CONTROLS_sweep_with_flaps_EQ",     1 },
	{ XINT, "SPECIAL_CONTROLS_flaps_with_gear_EQ",      1 },
	{ XINT, "SPECIAL_CONTROLS_slat_with_stall_EQ",      1 },
	{ XINT, "SPECIAL_CONTROLS_anti_ice_EQ",             1 },
	{ XINT, "SPECIAL_CONTROLS_arresting_EQ",            1 },
	{ XINT, "SPECIAL_CONTROLS_revt_on_td_EQ",           1 },
	{ XINT, "SPECIAL_CONTROLS_warn_gear_EQ",            1 },
	{ XINT, "SPECIAL_CONTROLS_warn_lorot_EQ",           1 },
	{ XINT, "SPECIAL_CONTROLS_auto_trim_EQ",            1 },
	{ XINT, "SPECIAL_CONTROLS_flaps_with_vec_EQ",       1 },
	{ XFLT, "SPECIAL_CONTROLS_brake_area",              1 },
	{ XFLT, "SPECIAL_CONTROLS_brake_Y",                 1 },
	{ XFLT, "SPECIAL_CONTROLS_brake_Z",                 1 },
	{ XFLT, "SPECIAL_CONTROLS_chute_area",              1 },
	{ XFLT, "SPECIAL_CONTROLS_chute_Y",                 1 },
	{ XFLT, "SPECIAL_CONTROLS_chute_Z",                 1 },
	{ XINT, "VTOL_CONTROL_vect_EQ",                     1 },
	{ XINT, "VTOL_CONTROL_auto_rpm_with_tvec",          1 },
	{ XINT, "VTOL_CONTROL_hide_prop_at_90_vect",        1 },
	{ XFLT, "VTOL_CONTROL_vect_rate",                   1 },
	{ XFLT, "VTOL_CONTROL_vect_min_disc",               1 },
	{ XFLT, "VTOL_CONTROL_vect_max_disc",               1 },
	{ XFLT, "VTOL_CONTROL_vectarmY",                    1 },
	{ XFLT, "VTOL_CONTROL_vectarmZ",                    1 },
	{ XFLT, "VTOL_CONTROL_cyclic_def_elev",             1 },
	{ XFLT, "VTOL_CONTROL_cyclic_def_ailn",             1 },
	{ XFLT, "VTOL_CONTROL_flap_arm",                    1 },
	{ XFLT, "VTOL_CONTROL_delta3",                      1 },
	{ XFLT, "VTOL_CONTROL_puff_LMN",                    3 },
	{ XFLT, "VTOL_CONTROL_puff_xyz",                    3 },
	{ XFLT, "VTOL_CONTROL_stab_delinc_to_Vne",          1 },
	{ XFLT, "VTOL_CONTROL_tail_with_coll",              1 },
	{ XFLT, "VTOL_CONTROL_diff_coll_with_roll",         1 },
	{ XFLT, "VTOL_CONTROL_diff_coll_with_hdng",         1 },
	{ XFLT, "VTOL_CONTROL_diff_coll_with_ptch",         1 },
	{ XFLT, "VTOL_CONTROL_diff_cycl_with_hdng_lon",     1 },
	{ XFLT, "VTOL_CONTROL_diff_cycl_with_hdng_lat",     1 },
	{ XFLT, "VTOL_CONTROL_rotor_trim_max_fwd",          1 },
	{ XFLT, "VTOL_CONTROL_rotor_trim_max_aft",          1 },
	{ XFLT, "ART_STAB_AShiV_old_all",                   1 },
	{ XFLT, "ART_STAB_ASloV_old_all",                   1 },
	{ XFLT, "ART_STAB_ASlo_max_thedot",                 1 },
	{ XFLT, "ART_STAB_ASlo_thedot_k",                   1 },
	{ XFLT, "ART_STAB_ASlo_max_psidot",                 1 },
	{ XFLT, "ART_STAB_ASlo_psidot_k",                   1 },
	{ XFLT, "ART_STAB_ASlo_max_phidot",                 1 },
	{ XFLT, "ART_STAB_ASlo_phidot_k",                   1 },
	{ XFLT, "ART_STAB_AShi_max_G",                      1 },
	{ XFLT, "ART_STAB_AShi_G_k",                        1 },
	{ XFLT, "ART_STAB_AShi_Gdot_k",                     1 },
	{ XFLT, "ART_STAB_AShi_max_alpha",                  1 },
	{ XFLT, "ART_STAB_AShi_alpha_k",                    1 },
	{ XFLT, "ART_STAB_AShi_alphadot_k",                 1 },
	{ XFLT, "ART_STAB_AShi_max_beta",                   1 },
	{ XFLT, "ART_STAB_AShi_beta_k",                     1 },
	{ XFLT, "ART_STAB_AShi_betadot_k",                  1 },
	{ XFLT, "ART_STAB_AShi_max_phidot",                 1 },
	{ XFLT, "ART_STAB_AShi_phidot_k",                   1 },
	{ XCHR, "WEARONS_wpn_name",                      12000 },
	{ XFLT, "WEARONS_x_wpn_att",                       24 },
	{ XFLT, "WEARONS_y_wpn_att",                       24 },
	{ XFLT, "WEARONS_z_wpn_att",                       24 },
	{ XFLT, "AUTO_SETS_est_Vs_msc",                     1 },
	{ XFLT, "AUTO_SETS_size_x",                         1 },
	{ XFLT, "AUTO_SETS_size_z",                         1 },
	{ XFLT, "AUTO_SETS_tire_s_contact",                 1 },
	{ XFLT, "AUTO_SETS_m_displaced_y",                  1 },
	{ XFLT, "AUTO_SETS_h_eqlbm",                        1 },
	{ XFLT, "AUTO_SETS_the_eqlbm",                      1 },
	{ XINT, "AUTO_SETS_steer_gear",                     1 },
	{ XINT, "AUTO_SETS_is_skid_gear",                   1 },
	{ XINT, "AUTO_SETS_dummy3",                         7 },
	{ XINT, "AUTO_SETS_has_radar",                      1 },
	{ XINT, "AUTO_SETS_has_SC_fd",                      1 },
	{ XINT, "AUTO_SETS_has_DC_fd",                      1 },
	{ XINT, "AUTO_SETS_has_stallwarn",                  1 },
	{ XINT, "AUTO_SETS_has_clutchswitch",               1 },
	{ XINT, "AUTO_SETS_has_prerotate",                  1 },
	{ XINT, "AUTO_SETS_has_idlespeed",                  1 },
	{ XINT, "AUTO_SETS_has_FADECswitch",                1 },
	{ XINT, "AUTO_SETS_has_litemap_tex_1",              1 },
	{ XINT, "AUTO_SETS_has_tailrotor",                  1 },
	{ XINT, "AUTO_SETS_has_collective",                 1 },
	{ XFLT, "SOUND_REF_snd_kias",                       1 },
	{ XFLT, "SOUND_REF_snd_rpm_prp",                    1 },
	{ XFLT, "SOUND_REF_snd_rpm_eng",                    1 },
	{ XFLT, "SOUND_REF_snd_n1",                         1 },
	{ XFLT, "VAR_INCIDENCE_inc2",                      73 },
	{ XFLT, "VAR_INCIDENCE_incnow",                    73 },
	{ XINT, "VAR_INCIDENCE_varinc",                    73 },
	{ XINT, "VAR_INCIDENCE_varincEQ",                   1 },
	{ XFLT, "OVERFLOW_rudd_with_ailn",                  1 },
	{ XFLT, "OVERFLOW_strut_comp",                     73 },
	{ XINT, "OVERFLOW_is_left",                        73 },
	{ XFLT, "OVERFLOW_lat_sign",                       73 },
	{ XINT, "OVERFLOW_jett_is_acf",                     1 },
	{ XINT, "OVERFLOW_collective_en",                   1 },
	{ XINT, "OVERFLOW_is_flying_stab",                  1 },
	{ XFLT, "OVERFLOW_dummy4",                          7 },
	{ XFLT, "OVERFLOW_diff_thro_ptch",                  1 },
	{ XFLT, "OVERFLOW_diff_thro_roll",                  1 },
	{ XINT, "OVERFLOW_phase_roll_tvect_in_at_90",       1 },
	{ XINT, "OVERFLOW_phase_roll_tvect_in_at_00",       1 },
	{ XINT, "OVERFLOW_phase_hdng_tvect_in_at_90",       1 },
	{ XINT, "OVERFLOW_phase_hdng_tvect_in_at_00",       1 },
	{ XINT, "OVERFLOW_has_asi_set",                     1 },
	{ XINT, "OVERFLOW_has_hdg_set",                     1 },
	{ XINT, "OVERFLOW_has_alt_set",                     1 },
	{ XFLT, "OVERFLOW_ASlo_the_V",                      1 },
	{ XFLT, "OVERFLOW_ASlo_psi_V",                      1 },
	{ XFLT, "OVERFLOW_ASlo_phi_V",                      1 },
	{ XFLT, "OVERFLOW_AShi_the_V",                      1 },
	{ XFLT, "OVERFLOW_AShi_psi_V",                      1 },
	{ XFLT, "OVERFLOW_AShi_phi_V",                      1 },
	{ XFLT, "OVERFLOW_spo1_vmax",                       1 },
	{ XFLT, "OVERFLOW_max_boost_pas",                   1 },
	{ XFLT, "OVERFLOW_min_trim_elev",                   1 },
	{ XFLT, "OVERFLOW_max_trim_elev",                   1 },
	{ XFLT, "OVERFLOW_min_trim_ailn",                   1 },
	{ XFLT, "OVERFLOW_max_trim_ailn",                   1 },
	{ XFLT, "OVERFLOW_min_trim_rudd",                   1 },
	{ XFLT, "OVERFLOW_max_trim_rudd",                   1 },
	{ XFLT, "OVERFLOW_lan_lite_psi_ref",                1 },
	{ XINT, "OVERFLOW_has_mixture",                     1 },
	{ XFLT, "OVERFLOW_TR",                             73 },
	{ XINT, "OVERFLOW_has_gear",                        1 },
	{ XINT, "OVERFLOW_cus_non_lin",                    50 },
	{ XFLT, "OVERFLOW_cus_doub_val",                   50 },
	{ XINT, "OVERFLOW_beaconEQ",                        1 },
	{ XINT, "OVERFLOW_has_knots_mach",                  1 },
	{ XFLT, "OVERFLOW_elev_trim_speedrat",              1 },
	{ XFLT, "OVERFLOW_ailn_trim_speedrat",              1 },
	{ XFLT, "OVERFLOW_rudd_trim_speedrat",              1 },
	{ XFLT, "OVERFLOW_disp_rat",                        1 },
	{ XFLT, "OVERFLOW_exhaust_rat",                     1 },
	{ XINT, "OVERFLOW_lo_speed_is_position",            1 },
	{ XFLT, "OVERFLOW_ASlo_max_the",                    1 },
	{ XFLT, "OVERFLOW_ASlo_the_k",                      1 },
	{ XFLT, "OVERFLOW_ASlo_max_phi",                    1 },
	{ XFLT, "OVERFLOW_ASlo_phi_k",                      1 },
	{ XINT, "OVERFLOW_is_ducted",                       8 },
	{ XFLT, "OVERFLOW_the_wpn_att",                    24 },
	{ XFLT, "OVERFLOW_psi_wpn_att",                    24 },
	{ XFLT, "OVERFLOW_big_panel_pix_default",           1 },
	{ XFLT, "OVERFLOW_HUD_ctr_y",                       9 },
	{ XINT, "OVERFLOW_spo2",                          730 },
	{ XFLT, "OVERFLOW_spo2_elR",                       73 },
	{ XFLT, "OVERFLOW_spo2_elT",                       73 },
	{ XFLT, "OVERFLOW_spo2_cratR",                      1 },
	{ XFLT, "OVERFLOW_spo2_cratT",                      1 },
	{ XFLT, "OVERFLOW_spo2_up",                         1 },
	{ XFLT, "OVERFLOW_spo2_vmax",                       1 },
	{ XFLT, "OVERFLOW_ail1_vmax",                       1 },
	{ XFLT, "OVERFLOW_roll_to_eng_spo1",                1 },
	{ XFLT, "OVERFLOW_roll_to_eng_spo2",                1 },
	{ XFLT, "OVERFLOW_dummy2",                         73 },
	{ XFLT, "OVERFLOW_EPR_max",                         1 },
	{ XINT, "OVERFLOW_sweep_with_vect_EQ",              1 },
	{ XINT, "OVERFLOW_old_cus_layers",                  1 },
	{ XINT, "OVERFLOW_has_litemap_tex_2",               1 },
	{ XFLT, "OVERFLOW_disc_tilt_elev",                  1 },
	{ XFLT, "OVERFLOW_disc_tilt_ailn",                  1 },
	{ XFLT, "OVERFLOW_lan_lite_psi_off",                1 },
	{ XFLT, "OVERFLOW_lan_lite_the_off",                1 },
	{ XFLT, "OVERFLOW_inertia_rat_prop",                1 },
	{ XFLT, "OVERFLOW_fuel_intro_time_jet",             1 },
	{ XFLT, "OVERFLOW_tire_mi",                        73 },
	{ XFLT, "OVERFLOW_vect_min_nace",                   1 },
	{ XFLT, "OVERFLOW_vect_max_nace",                   1 },
	{ XINT, "OVERFLOW_manual_rad_gyr",                  1 },
	{ XFLT, "OVERFLOW_max_ITT_C",                       1 },
	{ XFLT, "OVERFLOW_max_EGT_C",                       1 },
	{ XFLT, "OVERFLOW_fuel_intro_time_prop",            1 },
	{ XFLT, "OVERFLOW_spool_time_jet",                  1 },
	{ XFLT, "OVERFLOW_takeoff_trim",                    1 },
	{ XFLT, "OVERFLOW_average_MAC",                     1 },
	{ XINT, "OVERFLOW_custom_autopilot",                1 },
	{ XFLT, "OVERFLOW_ott_asi_ratio",                   1 },
	{ XFLT, "OVERFLOW_ott_asi_sec_into_future",         1 },
	{ XFLT, "OVERFLOW_ott_asi_kts_off_for_full_def",    1 },
	{ XFLT, "OVERFLOW_ott_phi_ratio",                   1 },
	{ XFLT, "OVERFLOW_ott_phi_sec_into_future",         1 },
	{ XFLT, "OVERFLOW_ott_phi_deg_off_for_full_def",    1 },
	{ XFLT, "OVERFLOW_ott_phi_sec_to_tune",             1 },
	{ XFLT, "OVERFLOW_ott_def_sec_into_future",         1 },
	{ XFLT, "OVERFLOW_ott_def_dot_off_for_full_def",    1 },
	{ XFLT, "OVERFLOW_ott_def_sec_to_tune",             1 },
	{ XFLT, "OVERFLOW_ott_the_ratio",                   1 },
	{ XFLT, "OVERFLOW_ott_the_sec_into_future",         1 },
	{ XFLT, "OVERFLOW_ott_the_deg_off_for_full_def",    1 },
	{ XFLT, "OVERFLOW_ott_the_sec_to_tune",             1 },
	{ XINT, "OVERFLOW_xint_overflow",                 206 },
	{ XCHR, "OVERFLOW_is_hm",                           1 },
	{ XCHR, "OVERFLOW_is_ga",                           1 },
	{ XCHR, "OVERFLOW_ICAO",                           40 },
	{ XCHR, "OVERFLOW_xchr_overflow",                 958 },
	{ XCHR, "end_of_data",                              1 },

	{ XEOF, NULL, 0 }

};

#endif

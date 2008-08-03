#ifndef _IMP_ACF_DEF_ACF625
#define _IMP_ACF_DEF_ACF625

static const AcfDef acf_def_acf625[] = {
	{ XCHR, "VIEW__PlatForm",                           1 },
	{ XCHR, "VIEW__Filler",                             4 },
	{ XCHR, "VIEW__tailnum",                           40 },
	{ XCHR, "VIEW__author",                           500 },
	{ XCHR, "VIEW__descrip",                          500 },
	{ XFLT, "VIEW__size_x",                             1 },
	{ XFLT, "VIEW__size_z",                             1 },
	{ XINT, "VIEW__is_APL",                             1 },
	{ XINT, "VIEW__actype",                             1 },
	{ XINT, "VIEW__HUDtype",                            1 },
	{ XINT, "VIEW__asi_kts",                            1 },
	{ XINT, "VIEW__cockpit_type",                       1 },
	{ XINT, "VIEW__has_fd",                             1 },
	{ XINT, "VIEW__has_stallwarn",                      1 },
	{ XINT, "VIEW__has_litemap_tex",                    1 },
	{ XFLT, "VIEW__peX",                                1 },
	{ XFLT, "VIEW__peY",                                1 },
	{ XFLT, "VIEW__peZ",                                1 },
	{ XFLT, "VIEW__Vso",                                1 },
	{ XFLT, "VIEW__Vs",                                 1 },
	{ XFLT, "VIEW__Vfe",                                1 },
	{ XFLT, "VIEW__Vno",                                1 },
	{ XFLT, "VIEW__Vne",                                1 },
	{ XFLT, "VIEW__Mmo",                                1 },
	{ XFLT, "VIEW__Gneg",                               1 },
	{ XFLT, "VIEW__Gpos",                               1 },
	{ XFLT, "VIEW__est_Vs",                             1 },
	{ XFLT, "VIEW__yawstringx",                         1 },
	{ XFLT, "VIEW__yawstringy",                         1 },
	{ XFLT, "VIEW__HUD_cntr",                           1 },
	{ XFLT, "VIEW__HUD_cntry",                          1 },
	{ XFLT, "VIEW__HUD_del",                            1 },
	{ XFLT, "VIEW__HUD_dely",                           1 },
	{ XINT, "VIEW__has_lanlite1",                       1 },
	{ XFLT, "VIEW__lanlite1X",                          1 },
	{ XFLT, "VIEW__lanlite1Y",                          1 },
	{ XFLT, "VIEW__lanlite1Z",                          1 },
	{ XFLT, "VIEW__lanlite_the",                        1 },
	{ XINT, "VIEW__has_lanlite2",                       1 },
	{ XFLT, "VIEW__lanlite2X",                          1 },
	{ XFLT, "VIEW__lanlite2Y",                          1 },
	{ XFLT, "VIEW__lanlite2Z",                          1 },
	{ XINT, "VIEW__has_lanlite3",                       1 },
	{ XFLT, "VIEW__lanlite3X",                          1 },
	{ XFLT, "VIEW__lanlite3Y",                          1 },
	{ XFLT, "VIEW__lanlite3Z",                          1 },
	{ XINT, "VIEW__has_fuserb1",                        1 },
	{ XFLT, "VIEW__fuserb1",                            1 },
	{ XFLT, "VIEW__fuserb1",                            1 },
	{ XFLT, "VIEW__fuserb1Z",                           1 },
	{ XINT, "VIEW__has_fuserb2",                        1 },
	{ XFLT, "VIEW__fuserb2",                            1 },
	{ XFLT, "VIEW__fuserb2",                            1 },
	{ XFLT, "VIEW__fuserb2Z",                           1 },
	{ XINT, "VIEW__has_taillite",                       1 },
	{ XFLT, "VIEW__tailliteX",                          1 },
	{ XFLT, "VIEW__tailliteY",                          1 },
	{ XFLT, "VIEW__tailliteZ",                          1 },
	{ XINT, "VIEW__has_navlites",                       1 },
	{ XINT, "PANEL_ins_type",                         200 },
	{ XFLT, "PANEL_ins_size",                         200 },
	{ XFLT, "PANEL_ins_x",                            200 },
	{ XFLT, "PANEL_ins_y",                            200 },
	{ XFLT, "PANEL_ins_delx",                         200 },
	{ XFLT, "PANEL_ins_dely",                         200 },
	{ XINT, "FORCE_Hydraulic",                          1 },
	{ XINT, "FORCE_Stickshaker",                        1 },
	{ XINT, "ENG___number_engines",                     1 },
	{ XINT, "ENG___auto_feather_EQ",                    1 },
	{ XFLT, "ENG___throtmax_FW",                        1 },
	{ XFLT, "ENG___throtmax_REV",                       1 },
	{ XFLT, "ENG___omegaminRS",                         1 },
	{ XFLT, "ENG___omegamaxRSC",                        1 },
	{ XFLT, "ENG___omegamingreenRSC",                   1 },
	{ XFLT, "ENG___omegamaxgreenRSC",                   1 },
	{ XFLT, "ENG___pmax",                               1 },
	{ XFLT, "ENG___tmax",                               1 },
	{ XFLT, "ENG___burnerinc",                          1 },
	{ XFLT, "ENG___critalt",                            1 },
	{ XFLT, "ENG___mpmax",                              1 },
	{ XFLT, "ENG___gearrat",                            1 },
	{ XFLT, "ENG___face_jet",                           1 },
	{ XFLT, "ENG___face_rocket",                        1 },
	{ XFLT, "ENG___spooltime",                          1 },
	{ XFLT, "ENG___max_mach_eff",                       1 },
	{ XFLT, "ENG___fmax_sl",                            1 },
	{ XFLT, "ENG___fmax_opt",                           1 },
	{ XFLT, "ENG___fmax_vac",                           1 },
	{ XFLT, "ENG___h_opt",                              1 },
	{ XFLT, "ENG___tip_mach_des_50",                    1 },
	{ XFLT, "ENG___tip_mach_des_100",                   1 },
	{ XFLT, "ENG___rotor_mi_rat",                       1 },
	{ XFLT, "ENG___tip_weight",                         1 },
	{ XFLT, "ENG___trotor_rat",                         1 },
	{ XINT, "PROP__engine_type",                        8 },
	{ XINT, "PROP__revthrust_eq",                       8 },
	{ XINT, "PROP__prop_type",                          8 },
	{ XFLT, "PROP__prop_dir",                           8 },
	{ XFLT, "PROP__num_blades",                         8 },
	{ XFLT, "PROP__SFC",                                8 },
	{ XFLT, "PROP__minpitch",                           8 },
	{ XFLT, "PROP__maxpitch",                           8 },
	{ XFLT, "PROP__revpitch",                           8 },
	{ XFLT, "PROP__sidecant",                           8 },
	{ XFLT, "PROP__vertcant",                           8 },
	{ XFLT, "PROP__miprop_rpm",                         8 },
	{ XFLT, "PROP__miprop_pitch",                       8 },
	{ XFLT, "PROP__discarea",                           8 },
	{ XFLT, "PROP__ringarea",                          80 },
	{ XFLT, "PROP__bladesweep",                        80 },
	{ XFLT, "PROP__des_rpm_prp",                        8 },
	{ XFLT, "PROP__des_kts_prp",                        8 },
	{ XFLT, "PROP__des_kts_acf",                        8 },
	{ XINT, "PARTS_part_eq",                           57 },
	{ XCHR, "PARTS_Rafl0",                           2280 },
	{ XCHR, "PARTS_Rafl1",                           2280 },
	{ XCHR, "PARTS_Tafl0",                           2280 },
	{ XCHR, "PARTS_Tafl1",                           2280 },
	{ XINT, "PARTS_els",                               57 },
	{ XFLT, "PARTS_Xarm",                              57 },
	{ XFLT, "PARTS_Yarm",                              57 },
	{ XFLT, "PARTS_Zarm",                              57 },
	{ XFLT, "PARTS_X_body_aero",                       57 },
	{ XFLT, "PARTS_Y_body_aero",                       57 },
	{ XFLT, "PARTS_Z_body_aero",                       57 },
	{ XFLT, "PARTS_Croot",                             57 },
	{ XFLT, "PARTS_Ctip",                              57 },
	{ XFLT, "PARTS_dihed",                             57 },
	{ XFLT, "PARTS_sweep1",                            57 },
	{ XFLT, "PARTS_sweep2",                            57 },
	{ XFLT, "PARTS_sweepnow",                          57 },
	{ XINT, "PARTS_varsweepEQ",                        57 },
	{ XFLT, "PARTS_semilen_SEG",                       57 },
	{ XFLT, "PARTS_semilen_JND",                       57 },
	{ XFLT, "PARTS_e",                                 57 },
	{ XFLT, "PARTS_AR",                                57 },
	{ XFLT, "PARTS_al_D_al0",                          57 },
	{ XFLT, "PARTS_cl_D_cl0",                          57 },
	{ XFLT, "PARTS_cm_D_cm0",                          57 },
	{ XFLT, "PARTS_delta_fac",                         57 },
	{ XFLT, "PARTS_spec_wash",                         57 },
	{ XFLT, "PARTS_alpha_max",                         57 },
	{ XFLT, "PARTS_slat_effect",                       57 },
	{ XFLT, "PARTS_s",                                570 },
	{ XFLT, "PARTS_mac",                              570 },
	{ XFLT, "PARTS_anginc",                           570 },
	{ XINT, "PARTS_ail1",                             570 },
	{ XINT, "PARTS_ail2",                             570 },
	{ XINT, "PARTS_splr",                             570 },
	{ XINT, "PARTS_flap",                             570 },
	{ XINT, "PARTS_flapEQ",                             1 },
	{ XINT, "PARTS_slat",                             570 },
	{ XINT, "PARTS_slatEQ",                             1 },
	{ XINT, "PARTS_sbrk",                             570 },
	{ XINT, "PARTS_sbrkEQ",                             1 },
	{ XINT, "PARTS_drud",                             570 },
	{ XINT, "PARTS_elev",                             570 },
	{ XINT, "PARTS_rudd",                             570 },
	{ XINT, "PARTS_in_downwash",                     32490 },
	{ XFLT, "PARTS_body_r",                            57 },
	{ XFLT, "PARTS_body_X",                          20520 },
	{ XFLT, "PARTS_body_Y",                          20520 },
	{ XFLT, "PARTS_body_Z",                          20520 },
	{ XINT, "PARTS_gear_type",                         57 },
	{ XFLT, "PARTS_gear_latE",                         57 },
	{ XFLT, "PARTS_gear_lonE",                         57 },
	{ XFLT, "PARTS_gear_axiE",                         57 },
	{ XFLT, "PARTS_gear_latR",                         57 },
	{ XFLT, "PARTS_gear_lonR",                         57 },
	{ XFLT, "PARTS_gear_axiR",                         57 },
	{ XFLT, "PARTS_gear_latN",                         57 },
	{ XFLT, "PARTS_gear_lonN",                         57 },
	{ XFLT, "PARTS_gear_axiN",                         57 },
	{ XFLT, "PARTS_gear_leglen",                       57 },
	{ XFLT, "PARTS_gear_tirrad",                       57 },
	{ XFLT, "PARTS_gearcon",                           57 },
	{ XFLT, "PARTS_geardmp",                           57 },
	{ XFLT, "PARTS_gearstatdef",                       57 },
	{ XFLT, "PARTS_gear_def",                          57 },
	{ XFLT, "PARTS_gear_deploy",                       57 },
	{ XFLT, "PARTS_gear_xnodef",                       57 },
	{ XFLT, "PARTS_gear_ynodef",                       57 },
	{ XFLT, "PARTS_gear_znodef",                       57 },
	{ XFLT, "BODY__fuse_cd",                            1 },
	{ XFLT, "CTRLS_ail1_crat",                          1 },
	{ XFLT, "CTRLS_ail1_up",                            1 },
	{ XFLT, "CTRLS_ail1_d",                             1 },
	{ XFLT, "CTRLS_lail1def",                           1 },
	{ XFLT, "CTRLS_rail1def",                           1 },
	{ XFLT, "CTRLS_ail2_crat",                          1 },
	{ XFLT, "CTRLS_ail2_up",                            1 },
	{ XFLT, "CTRLS_ail2_d",                             1 },
	{ XFLT, "CTRLS_lail2def",                           1 },
	{ XFLT, "CTRLS_rail2def",                           1 },
	{ XFLT, "CTRLS_elev_crat",                          1 },
	{ XFLT, "CTRLS_elev_up",                            1 },
	{ XFLT, "CTRLS_elev_d",                             1 },
	{ XFLT, "CTRLS_lhstbdef",                           1 },
	{ XFLT, "CTRLS_rhstbdef",                           1 },
	{ XFLT, "CTRLS_rudd_crat",                          1 },
	{ XFLT, "CTRLS_rudd_l",                             1 },
	{ XFLT, "CTRLS_rudd1def",                           1 },
	{ XFLT, "CTRLS_rudd2def",                           1 },
	{ XFLT, "CTRLS_splr_crat",                          1 },
	{ XFLT, "CTRLS_splr_u",                             1 },
	{ XFLT, "CTRLS_lsplrdef",                           1 },
	{ XFLT, "CTRLS_rsplrdef",                           1 },
	{ XFLT, "CTRLS_drud_crat",                          1 },
	{ XFLT, "CTRLS_drud_u",                             1 },
	{ XFLT, "CTRLS_ldruddef",                           1 },
	{ XFLT, "CTRLS_rdruddef",                           1 },
	{ XFLT, "CTRLS_sbrk_crat",                          1 },
	{ XFLT, "CTRLS_sbrk_up",                            1 },
	{ XFLT, "CTRLS_flap_crat",                          1 },
	{ XFLT, "CTRLS_flap_dn",                            7 },
	{ XFLT, "CTRLS_lflapdef",                           1 },
	{ XFLT, "CTRLS_rflapdef",                           1 },
	{ XFLT, "CTRLS_hstb_trim_up",                       1 },
	{ XFLT, "CTRLS_hstb_trim_d",                        1 },
	{ XFLT, "CTRLS_hstb_trim_def",                      1 },
	{ XINT, "CTRLS_hstabtype",                          1 },
	{ XINT, "CTRLS_vstabtype",                          1 },
	{ XINT, "CTRLS_flap_type",                          1 },
	{ XINT, "CTRLS_con_smooth",                         1 },
	{ XFLT, "CTRLS_flap_cl",                            1 },
	{ XFLT, "CTRLS_flap_cd",                            1 },
	{ XFLT, "CTRLS_flap_cm",                            1 },
	{ XINT, "CTRLS_flap_detents",                       1 },
	{ XFLT, "CTRLS_flap_deftime",                       1 },
	{ XFLT, "CTRLS_slat_inc",                           1 },
	{ XFLT, "CTRLS_blownflap_addspeed",                 1 },
	{ XFLT, "CTRLS_blownflap_throtred",                 1 },
	{ XFLT, "CTRLS_blownflap_minengag",                 1 },
	{ XINT, "GEAR__gear_retract",                       1 },
	{ XINT, "GEAR__is_taildragger",                     1 },
	{ XINT, "GEAR__gear_is_skid",                       1 },
	{ XFLT, "GEAR__gear_deptime",                       1 },
	{ XFLT, "GEAR__nwsteerdeg1",                        1 },
	{ XFLT, "GEAR__nwsteerdeg2",                        1 },
	{ XFLT, "GEAR__nwsteerspd",                         1 },
	{ XFLT, "GEAR__water_rud_longarm",                  1 },
	{ XFLT, "GEAR__water_rud_area",                     1 },
	{ XFLT, "GEAR__water_rud_maxdef",                   1 },
	{ XFLT, "GEAR__h_eqlbm",                            1 },
	{ XFLT, "GEAR__the_eqlbm",                          1 },
	{ XINT, "GEAR__gear_door_use",                     10 },
	{ XFLT, "GEAR__gear_door_loc",                     30 },
	{ XFLT, "GEAR__gear_door_geo",                    120 },
	{ XFLT, "GEAR__gear_door_axi_rot",                 10 },
	{ XFLT, "GEAR__gear_door_ext_ang",                 10 },
	{ XFLT, "GEAR__gear_door_ret_ang",                 10 },
	{ XFLT, "GEAR__gear_door_ang_now",                 10 },
	{ XFLT, "WEIGHTcgY",                                1 },
	{ XFLT, "WEIGHTcgZ",                                1 },
	{ XFLT, "WEIGHTJxx_unitmass",                       1 },
	{ XFLT, "WEIGHTJyy_unitmass",                       1 },
	{ XFLT, "WEIGHTJzz_unitmass",                       1 },
	{ XFLT, "WEIGHTm_empty",                            1 },
	{ XFLT, "WEIGHTm_displaced",                        1 },
	{ XFLT, "WEIGHTm_max",                              1 },
	{ XFLT, "WEIGHTm_fuel",                             1 },
	{ XFLT, "WEIGHTm_jettison",                         1 },
	{ XFLT, "WEIGHTm_displaced_y",                      1 },
	{ XFLT, "SPEC__jato_theta",                         1 },
	{ XFLT, "SPEC__jato_thrust",                        1 },
	{ XFLT, "SPEC__jato_dur",                           1 },
	{ XFLT, "SPEC__jato_sfc",                           1 },
	{ XFLT, "SPEC__jato_Y",                             1 },
	{ XFLT, "SPEC__jato_Z",                             1 },
	{ XFLT, "SPEC__brake_area",                         1 },
	{ XFLT, "SPEC__brake_Y",                            1 },
	{ XFLT, "SPEC__brake_Z",                            1 },
	{ XFLT, "SPEC__chute_area",                         1 },
	{ XFLT, "SPEC__chute_Y",                            1 },
	{ XFLT, "SPEC__chute_Z",                            1 },
	{ XFLT, "SPEC__ail1pitch",                          1 },
	{ XFLT, "SPEC__ail1flaps",                          1 },
	{ XFLT, "SPEC__ail2pitch",                          1 },
	{ XFLT, "SPEC__ail2flaps",                          1 },
	{ XFLT, "SPEC__stabroll",                           1 },
	{ XFLT, "SPEC__stabhdng",                           1 },
	{ XFLT, "SPEC__ail2vmax",                           1 },
	{ XFLT, "SPEC__tvec_ptch",                          1 },
	{ XFLT, "SPEC__tvec_roll",                          1 },
	{ XFLT, "SPEC__tvec_hdng",                          1 },
	{ XFLT, "SPEC__diff_thro_with_hdng",                1 },
	{ XINT, "SPEC__arrestingEQ",                        1 },
	{ XINT, "SPEC__antiiceEQ",                          1 },
	{ XINT, "SPEC__bitchinbettyEQ",                     1 },
	{ XINT, "SPEC__gearhornEQ",                         1 },
	{ XINT, "SPEC__autosbrkEQ",                         1 },
	{ XINT, "SPEC__autofbrkEQ",                         1 },
	{ XINT, "SPEC__autosweepEQ",                        1 },
	{ XINT, "SPEC__autoslatEQ",                         1 },
	{ XINT, "VTOL__vectEQ",                             1 },
	{ XFLT, "VTOL__vectarm",                            1 },
	{ XFLT, "VTOL__cyclic_elev",                        1 },
	{ XFLT, "VTOL__cyclic_ailn",                        1 },
	{ XFLT, "VTOL__delta3",                             1 },
	{ XFLT, "VTOL__puffL",                              1 },
	{ XFLT, "VTOL__puffM",                              1 },
	{ XFLT, "VTOL__puffN",                              1 },
	{ XFLT, "VTOL__tail_with_coll",                     1 },
	{ XFLT, "VTOL__diff_coll_with_roll",                1 },
	{ XFLT, "VTOL__diff_coll_with_hdng",                1 },
	{ XFLT, "VTOL__diff_cycl_with_hdng",                1 },
	{ XINT, "VTOL__auto_rpm_with_tvec",                 1 },
	{ XFLT, "VTOL__rotor_trim_max_fwd",                 1 },
	{ XFLT, "VTOL__rotor_trim_max_aft",                 1 },
	{ XFLT, "ASTAB_AShiV",                              1 },
	{ XFLT, "ASTAB_ASloV",                              1 },
	{ XFLT, "ASTAB_Asmaxp_lo",                          1 },
	{ XFLT, "ASTAB_Asp_lo_rate",                        1 },
	{ XFLT, "ASTAB_Asmaxp_hi",                          1 },
	{ XFLT, "ASTAB_Asp_hi_pos",                         1 },
	{ XFLT, "ASTAB_Asmaxh_lo",                          1 },
	{ XFLT, "ASTAB_Ash_lo_rate",                        1 },
	{ XFLT, "ASTAB_Asmaxh_hi",                          1 },
	{ XFLT, "ASTAB_Ash_hi_pos",                         1 },
	{ XFLT, "ASTAB_Asmaxr_lo",                          1 },
	{ XFLT, "ASTAB_Asr_lo_rate",                        1 },
	{ XFLT, "ASTAB_Asmaxr_hi",                          1 },
	{ XFLT, "ASTAB_Asr_hi_rate",                        1 },
	{ XFLT, "fdummy1",                                  1 },
	{ XFLT, "fdummy2",                                  1 },
	{ XFLT, "element_len",                             57 },
	{ XFLT, "gear_door_size",                           1 },
	{ XFLT, "stab_change_to_redline",                   1 },
	{ XFLT, "Vmca",                                     1 },
	{ XFLT, "Vyse",                                     1 },
	{ XFLT, "flap_arm",                                 1 },
	{ XFLT, "tank_xyz12",                               6 },
	{ XFLT, "cgZ_fwd",                                  1 },
	{ XFLT, "cgZ_aft",                                  1 },
	{ XFLT, "gear_cyc_time",                            5 },
	{ XFLT, "refuel_X",                                 1 },
	{ XFLT, "refuel_Y",                                 1 },
	{ XFLT, "refuel_Z",                                 1 },
	{ XCHR, "gear_steers",                             57 },
	{ XCHR, "hybrid_gear",                              1 },
	{ XCHR, "chr_overflow",                             2 },
	{ XINT, "vardihedEQ",                              57 },
	{ XFLT, "dihed2",                                  57 },
	{ XFLT, "dihednow",                                57 },
	{ XFLT, "jett_xyz",                                 3 },
	{ XFLT, "puffX",                                    1 },
	{ XFLT, "puffY",                                    1 },
	{ XFLT, "puffZ",                                    1 },
	{ XFLT, "Vle",                                      1 },
	{ XFLT, "ASp_hi_rate",                              1 },
	{ XFLT, "ASh_hi_rate",                              1 },
	{ XFLT, "spooltime_prop",                           1 },
	{ XFLT, "elevflaps",                                1 },
	{ XFLT, "tank_xyz3",                                3 },
	{ XFLT, "tank_rat",                                 3 },
	{ XFLT, "stall_warn_alpha",                         1 },
	{ XFLT, "mass_shift",                               1 },
	{ XFLT, "mass_shift_dx",                            1 },
	{ XFLT, "mass_shift_dz",                            1 },
	{ XFLT, "feathered_pitch",                          1 },
	{ XFLT, "ASmaxg_hi",                                1 },
	{ XFLT, "ASg_hi_pos",                               1 },
	{ XFLT, "ASg_hi_rate",                              1 },
	{ XFLT, "flt_overflow",                            11 },
	{ XINT, "OVER__drive_by_wire",                      1 },
	{ XINT, "is_glossy",                                1 },
	{ XINT, "num_tanks",                                1 },
	{ XINT, "has_refuel",                               1 },
	{ XINT, "draw_geo_frnt_views",                      1 },
	{ XINT, "draw_geo_side_views",                      1 },
	{ XINT, "jett_is_slung",                            1 },
	{ XFLT, "eng_mass",                                57 },
	{ XINT, "phase_tvect",                              1 },
	{ XINT, "auto_trim",                                1 },
	{ XINT, "has_DC_fd",                                1 },
	{ XINT, "int_overflow",                            23 },

	{ XEOF, NULL, 0 }
};

#endif /* _IMP_ACF_DEF_ACF625 */

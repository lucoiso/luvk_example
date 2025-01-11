;
; Note: shader requires additional functionality:
;       Use native low precision
;
; shader debug name: e0e8185b3d230c97c2697adcb23b0dc3.pdb
; shader hash: e0e8185b3d230c97c2697adcb23b0dc3
;
; Buffer Definitions:
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%0 = type { i32, i32, i32, i32 }
%___rpsl_node_info_struct = type <{ i32, i32, i32, i32, i32 }>
%___rpsl_entry_desc_struct = type <{ i32, i32, i32, i32, i8*, i8* }>
%___rpsl_type_info_struct = type <{ i8, i8, i8, i8, i32, i32, i32 }>
%___rpsl_params_info_struct = type <{ i32, i32, i32, i32, i32, i16, i16 }>
%___rpsl_shader_ref_struct = type <{ i32, i32, i32, i32 }>
%___rpsl_pipeline_info_struct = type <{ i32, i32, i32, i32 }>
%___rpsl_pipeline_field_info_struct = type <{ i32, i32, i32, i32, i32, i32, i32, i32 }>
%___rpsl_pipeline_res_binding_info_struct = type <{ i32, i32, i32, i32 }>
%___rpsl_module_info_struct = type <{ i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, [71 x i8]*, [3 x %___rpsl_node_info_struct]*, [3 x %___rpsl_type_info_struct]*, [5 x %___rpsl_params_info_struct]*, [2 x %___rpsl_entry_desc_struct]*, [1 x %___rpsl_shader_ref_struct]*, [1 x %___rpsl_pipeline_info_struct]*, [1 x %___rpsl_pipeline_field_info_struct]*, [1 x %___rpsl_pipeline_res_binding_info_struct]*, i32 }>
%struct.RpsParameterDesc = type { %struct.RpsTypeInfo, i32, %0*, i8*, i32 }
%struct.RpsTypeInfo = type { i16, i16 }
%struct.RpsNodeDesc = type { i32, i32, %struct.RpsParameterDesc*, i8* }
%struct.RpslEntry = type { i8*, void (i32, i8**, i32)*, %struct.RpsParameterDesc*, %struct.RpsNodeDesc*, i32, i32 }
%struct.texture = type { i32, i32, i32, i32, %struct.SubresourceRange, float, i32 }
%struct.SubresourceRange = type { i16, i16, i32, i32 }
%struct.ResourceDesc = type { i32, i32, i32, i32, i32, i32, i32, i32, i32 }

@___rpsl_nodedefs_rpsl_triangle = private constant [3 x %___rpsl_node_info_struct] [%___rpsl_node_info_struct <{ i32 0, i32 34, i32 0, i32 2, i32 1 }>, %___rpsl_node_info_struct <{ i32 1, i32 46, i32 2, i32 1, i32 1 }>, %___rpsl_node_info_struct zeroinitializer], align 4
@___rpsl_entries_rpsl_triangle = private constant [2 x %___rpsl_entry_desc_struct] [%___rpsl_entry_desc_struct <{ i32 0, i32 55, i32 3, i32 1, i8* bitcast (void (%struct.texture*)* @rpsl_M_rpsl_triangle_Fn_main to i8*), i8* bitcast (void (i32, i8**, i32)* @rpsl_M_rpsl_triangle_Fn_main_wrapper to i8*) }>, %___rpsl_entry_desc_struct zeroinitializer], align 4
@___rpsl_types_metadata_rpsl_triangle = private constant [3 x %___rpsl_type_info_struct] [%___rpsl_type_info_struct <{ i8 6, i8 0, i8 0, i8 0, i32 0, i32 36, i32 4 }>, %___rpsl_type_info_struct <{ i8 4, i8 32, i8 0, i8 4, i32 0, i32 16, i32 4 }>, %___rpsl_type_info_struct zeroinitializer], align 4
@___rpsl_params_metadata_rpsl_triangle = private constant [5 x %___rpsl_params_info_struct] [%___rpsl_params_info_struct <{ i32 14, i32 0, i32 272629888, i32 -1, i32 0, i16 36, i16 0 }>, %___rpsl_params_info_struct <{ i32 16, i32 1, i32 0, i32 -1, i32 0, i16 16, i16 36 }>, %___rpsl_params_info_struct <{ i32 21, i32 0, i32 128, i32 -1, i32 0, i16 36, i16 0 }>, %___rpsl_params_info_struct <{ i32 60, i32 0, i32 524288, i32 -1, i32 0, i16 36, i16 0 }>, %___rpsl_params_info_struct zeroinitializer], align 4
@___rpsl_shader_refs_rpsl_triangle = private constant [1 x %___rpsl_shader_ref_struct] zeroinitializer, align 4
@___rpsl_pipelines_rpsl_triangle = private constant [1 x %___rpsl_pipeline_info_struct] zeroinitializer, align 4
@___rpsl_pipeline_fields_rpsl_triangle = private constant [1 x %___rpsl_pipeline_field_info_struct] zeroinitializer, align 4
@___rpsl_pipeline_res_bindings_rpsl_triangle = private constant [1 x %___rpsl_pipeline_res_binding_info_struct] zeroinitializer, align 4
@___rpsl_string_table_rpsl_triangle = constant [71 x i8] c"rpsl_triangle\00t\00data\00renderTarget\00clear_color\00Triangle\00main\00backbuffer\00", align 4
@___rpsl_module_info_rpsl_triangle = dllexport constant %___rpsl_module_info_struct <{ i32 1297305682, i32 3, i32 9, i32 0, i32 71, i32 2, i32 2, i32 4, i32 1, i32 0, i32 0, i32 0, i32 0, [71 x i8]* @___rpsl_string_table_rpsl_triangle, [3 x %___rpsl_node_info_struct]* @___rpsl_nodedefs_rpsl_triangle, [3 x %___rpsl_type_info_struct]* @___rpsl_types_metadata_rpsl_triangle, [5 x %___rpsl_params_info_struct]* @___rpsl_params_metadata_rpsl_triangle, [2 x %___rpsl_entry_desc_struct]* @___rpsl_entries_rpsl_triangle, [1 x %___rpsl_shader_ref_struct]* @___rpsl_shader_refs_rpsl_triangle, [1 x %___rpsl_pipeline_info_struct]* @___rpsl_pipelines_rpsl_triangle, [1 x %___rpsl_pipeline_field_info_struct]* @___rpsl_pipeline_fields_rpsl_triangle, [1 x %___rpsl_pipeline_res_binding_info_struct]* @___rpsl_pipeline_res_bindings_rpsl_triangle, i32 1297305682 }>, align 4
@"@@rps_Str0" = private unnamed_addr constant [12 x i8] c"clear_color\00"
@"@@rps_Str1" = private unnamed_addr constant [2 x i8] c"t\00"
@"@@rps_ParamAttr2" = private constant %0 { i32 272629888, i32 0, i32 0, i32 0 }, align 4
@"@@rps_Str3" = private unnamed_addr constant [5 x i8] c"data\00"
@"@@rps_ParamAttr4" = private constant %0 { i32 0, i32 0, i32 27, i32 0 }, align 4
@"@@rps_ParamDescArray5" = private constant [2 x %struct.RpsParameterDesc] [%struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 36, i16 64 }, i32 0, %0* @"@@rps_ParamAttr2", i8* getelementptr inbounds ([2 x i8], [2 x i8]* @"@@rps_Str1", i32 0, i32 0), i32 4 }, %struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 16, i16 0 }, i32 0, %0* @"@@rps_ParamAttr4", i8* getelementptr inbounds ([5 x i8], [5 x i8]* @"@@rps_Str3", i32 0, i32 0), i32 0 }], align 4
@"@@rps_Str6" = private unnamed_addr constant [9 x i8] c"Triangle\00"
@"@@rps_Str7" = private unnamed_addr constant [13 x i8] c"renderTarget\00"
@"@@rps_ParamAttr8" = private constant %0 { i32 128, i32 0, i32 35, i32 0 }, align 4
@"@@rps_ParamDescArray9" = private constant [1 x %struct.RpsParameterDesc] [%struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 36, i16 64 }, i32 0, %0* @"@@rps_ParamAttr8", i8* getelementptr inbounds ([13 x i8], [13 x i8]* @"@@rps_Str7", i32 0, i32 0), i32 4 }], align 4
@NodeDecls_rpsl_triangle = dllexport constant [2 x %struct.RpsNodeDesc] [%struct.RpsNodeDesc { i32 1, i32 2, %struct.RpsParameterDesc* getelementptr inbounds ([2 x %struct.RpsParameterDesc], [2 x %struct.RpsParameterDesc]* @"@@rps_ParamDescArray5", i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @"@@rps_Str0", i32 0, i32 0) }, %struct.RpsNodeDesc { i32 1, i32 1, %struct.RpsParameterDesc* getelementptr inbounds ([1 x %struct.RpsParameterDesc], [1 x %struct.RpsParameterDesc]* @"@@rps_ParamDescArray9", i32 0, i32 0), i8* getelementptr inbounds ([9 x i8], [9 x i8]* @"@@rps_Str6", i32 0, i32 0) }], align 4
@"@@rps_Str10" = private unnamed_addr constant [5 x i8] c"main\00"
@"@@rps_Str11" = private unnamed_addr constant [11 x i8] c"backbuffer\00"
@"@@rps_ParamAttr12" = private constant %0 { i32 524288, i32 0, i32 0, i32 0 }, align 4
@"@@rps_ParamDescArray13" = private constant [1 x %struct.RpsParameterDesc] [%struct.RpsParameterDesc { %struct.RpsTypeInfo { i16 36, i16 64 }, i32 0, %0* @"@@rps_ParamAttr12", i8* getelementptr inbounds ([11 x i8], [11 x i8]* @"@@rps_Str11", i32 0, i32 0), i32 4 }], align 4
@"rpsl_M_rpsl_triangle_E_main@value" = constant %struct.RpslEntry { i8* getelementptr inbounds ([5 x i8], [5 x i8]* @"@@rps_Str10", i32 0, i32 0), void (i32, i8**, i32)* @rpsl_M_rpsl_triangle_Fn_main_wrapper, %struct.RpsParameterDesc* getelementptr inbounds ([1 x %struct.RpsParameterDesc], [1 x %struct.RpsParameterDesc]* @"@@rps_ParamDescArray13", i32 0, i32 0), %struct.RpsNodeDesc* getelementptr inbounds ([2 x %struct.RpsNodeDesc], [2 x %struct.RpsNodeDesc]* @NodeDecls_rpsl_triangle, i32 0, i32 0), i32 1, i32 2 }, align 4
@rpsl_M_rpsl_triangle_E_main = dllexport constant %struct.RpslEntry* @"rpsl_M_rpsl_triangle_E_main@value", align 4
@rpsl_M_rpsl_triangle_E_main_pp = dllexport constant %struct.RpslEntry** @rpsl_M_rpsl_triangle_E_main, align 4
@dx.nothing.a = internal constant [1 x i32] zeroinitializer

; Function Attrs: nounwind
define internal fastcc void @"\01?make_default_texture_view_from_desc@@YA?AUtexture@@IUResourceDesc@@@Z"(%struct.texture* noalias sret %agg.result, i32 %resourceHdl, %struct.ResourceDesc* %desc) #0 {
entry:
  %0 = alloca %struct.texture
  call void @llvm.dbg.declare(metadata %struct.ResourceDesc* %desc, metadata !148, metadata !149), !dbg !150 ; var:"desc" !DIExpression() func:"make_default_texture_view_from_desc"
  call void @llvm.dbg.value(metadata i32 %resourceHdl, i64 0, metadata !151, metadata !149), !dbg !152 ; var:"resourceHdl" !DIExpression() func:"make_default_texture_view_from_desc"
  %MipLevels = getelementptr inbounds %struct.ResourceDesc, %struct.ResourceDesc* %desc, i32 0, i32 6, !dbg !153 ; line:158 col:14
  %1 = load i32, i32* %MipLevels, align 4, !dbg !153 ; line:158 col:14
  %Type = getelementptr inbounds %struct.ResourceDesc, %struct.ResourceDesc* %desc, i32 0, i32 0, !dbg !154 ; line:157 col:15
  %2 = load i32, i32* %Type, align 4, !dbg !154 ; line:157 col:15
  %cmp = icmp eq i32 %2, 4, !dbg !155 ; line:157 col:20
  %tobool = icmp ne i1 %cmp, false, !dbg !155 ; line:157 col:20
  %tobool1 = icmp ne i1 %tobool, false, !dbg !155 ; line:157 col:20
  call void @llvm.dbg.declare(metadata %struct.texture* %0, metadata !156, metadata !149), !dbg !157 ; var:"result" !DIExpression() func:"make_default_texture_view"
  br i1 %tobool1, label %cond.true, label %cond.false, !dbg !159 ; line:157 col:9

cond.true:                                        ; preds = %entry
  br label %cond.end, !dbg !159 ; line:157 col:9

cond.false:                                       ; preds = %entry
  %DepthOrArraySize = getelementptr inbounds %struct.ResourceDesc, %struct.ResourceDesc* %desc, i32 0, i32 5, !dbg !160 ; line:157 col:54
  %3 = load i32, i32* %DepthOrArraySize, align 4, !dbg !160 ; line:157 col:54
  br label %cond.end, !dbg !159 ; line:157 col:9

cond.end:                                         ; preds = %cond.false, %cond.true
  %cond = phi i32 [ 1, %cond.true ], [ %3, %cond.false ], !dbg !159 ; line:157 col:9
  %4 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !161 ; line:154 col:12
  call void @llvm.dbg.value(metadata i32 %1, i64 0, metadata !162, metadata !149), !dbg !163 ; var:"numMips" !DIExpression() func:"make_default_texture_view"
  call void @llvm.dbg.value(metadata i32 %cond, i64 0, metadata !164, metadata !149), !dbg !165 ; var:"arraySlices" !DIExpression() func:"make_default_texture_view"
  call void @llvm.dbg.value(metadata i32 %resourceHdl, i64 0, metadata !166, metadata !149), !dbg !167 ; var:"resourceHdl" !DIExpression() func:"make_default_texture_view"
  %5 = bitcast %struct.texture* %0 to i8*, !dbg !161 ; line:154 col:12
  call void @llvm.memset.p0i8.i32(i8* %5, i8 0, i32 36, i32 4, i1 false) #0, !dbg !161, !noalias !168 ; line:154 col:12
  %Resource.i = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 0, !dbg !171 ; line:123 col:12
  %6 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !172 ; line:123 col:21
  store i32 %resourceHdl, i32* %Resource.i, align 4, !dbg !172, !noalias !168 ; line:123 col:21
  %ViewFormat.i = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 1, !dbg !173 ; line:124 col:12
  %7 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !174 ; line:124 col:23
  store i32 0, i32* %ViewFormat.i, align 4, !dbg !174, !noalias !168 ; line:124 col:23
  %TemporalLayer.i = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 2, !dbg !175 ; line:125 col:12
  %8 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !176 ; line:125 col:26
  store i32 0, i32* %TemporalLayer.i, align 4, !dbg !176, !noalias !168 ; line:125 col:26
  %Flags.i = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 3, !dbg !177 ; line:126 col:12
  %9 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !178 ; line:126 col:18
  store i32 0, i32* %Flags.i, align 4, !dbg !178, !noalias !168 ; line:126 col:18
  %base_mip_level.i5 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 4, i32 0, !dbg !179 ; line:127 col:29
  %10 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !180 ; line:127 col:44
  store i16 0, i16* %base_mip_level.i5, align 2, !dbg !180, !noalias !168 ; line:127 col:44
  %conv.i = trunc i32 %1 to i16, !dbg !181 ; line:128 col:56
  %mip_level_count.i4 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 4, i32 1, !dbg !182 ; line:128 col:29
  %11 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !183 ; line:128 col:45
  store i16 %conv.i, i16* %mip_level_count.i4, align 2, !dbg !183, !noalias !168 ; line:128 col:45
  %base_array_layer.i3 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 4, i32 2, !dbg !184 ; line:129 col:29
  %12 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !185 ; line:129 col:46
  store i32 0, i32* %base_array_layer.i3, align 4, !dbg !185, !noalias !168 ; line:129 col:46
  %array_layer_count.i2 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 4, i32 3, !dbg !186 ; line:130 col:29
  %13 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !187 ; line:130 col:47
  store i32 %cond, i32* %array_layer_count.i2, align 4, !dbg !187, !noalias !168 ; line:130 col:47
  %MinLodClamp.i = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 5, !dbg !188 ; line:131 col:12
  %14 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !189 ; line:131 col:24
  store float 0.000000e+00, float* %MinLodClamp.i, align 4, !dbg !189, !noalias !168 ; line:131 col:24
  %ComponentMapping.i = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 6, !dbg !190 ; line:132 col:12
  %15 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !191 ; line:132 col:29
  store i32 50462976, i32* %ComponentMapping.i, align 4, !dbg !191, !noalias !168 ; line:132 col:29
  %16 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !192 ; line:134 col:5
  %17 = getelementptr inbounds %struct.texture, %struct.texture* %agg.result, i32 0, i32 0, !dbg !192 ; line:134 col:5
  %18 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 0, !dbg !192 ; line:134 col:5
  %19 = load i32, i32* %18, !dbg !192 ; line:134 col:5
  store i32 %19, i32* %17, !dbg !192 ; line:134 col:5
  %20 = getelementptr inbounds %struct.texture, %struct.texture* %agg.result, i32 0, i32 1, !dbg !192 ; line:134 col:5
  %21 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 1, !dbg !192 ; line:134 col:5
  %22 = load i32, i32* %21, !dbg !192 ; line:134 col:5
  store i32 %22, i32* %20, !dbg !192 ; line:134 col:5
  %23 = getelementptr inbounds %struct.texture, %struct.texture* %agg.result, i32 0, i32 2, !dbg !192 ; line:134 col:5
  %24 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 2, !dbg !192 ; line:134 col:5
  %25 = load i32, i32* %24, !dbg !192 ; line:134 col:5
  store i32 %25, i32* %23, !dbg !192 ; line:134 col:5
  %26 = getelementptr inbounds %struct.texture, %struct.texture* %agg.result, i32 0, i32 3, !dbg !192 ; line:134 col:5
  %27 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 3, !dbg !192 ; line:134 col:5
  %28 = load i32, i32* %27, !dbg !192 ; line:134 col:5
  store i32 %28, i32* %26, !dbg !192 ; line:134 col:5
  %29 = getelementptr inbounds %struct.texture, %struct.texture* %agg.result, i32 0, i32 4, !dbg !192 ; line:134 col:5
  %30 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 4, !dbg !192 ; line:134 col:5
  %31 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %29, i32 0, i32 0, !dbg !192 ; line:134 col:5
  %32 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %30, i32 0, i32 0, !dbg !192 ; line:134 col:5
  %33 = load i16, i16* %32, !dbg !192 ; line:134 col:5
  store i16 %33, i16* %31, !dbg !192 ; line:134 col:5
  %34 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %29, i32 0, i32 1, !dbg !192 ; line:134 col:5
  %35 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %30, i32 0, i32 1, !dbg !192 ; line:134 col:5
  %36 = load i16, i16* %35, !dbg !192 ; line:134 col:5
  store i16 %36, i16* %34, !dbg !192 ; line:134 col:5
  %37 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %29, i32 0, i32 2, !dbg !192 ; line:134 col:5
  %38 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %30, i32 0, i32 2, !dbg !192 ; line:134 col:5
  %39 = load i32, i32* %38, !dbg !192 ; line:134 col:5
  store i32 %39, i32* %37, !dbg !192 ; line:134 col:5
  %40 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %29, i32 0, i32 3, !dbg !192 ; line:134 col:5
  %41 = getelementptr inbounds %struct.SubresourceRange, %struct.SubresourceRange* %30, i32 0, i32 3, !dbg !192 ; line:134 col:5
  %42 = load i32, i32* %41, !dbg !192 ; line:134 col:5
  store i32 %42, i32* %40, !dbg !192 ; line:134 col:5
  %43 = getelementptr inbounds %struct.texture, %struct.texture* %agg.result, i32 0, i32 5, !dbg !192 ; line:134 col:5
  %44 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 5, !dbg !192 ; line:134 col:5
  %45 = load float, float* %44, !dbg !192 ; line:134 col:5
  store float %45, float* %43, !dbg !192 ; line:134 col:5
  %46 = getelementptr inbounds %struct.texture, %struct.texture* %agg.result, i32 0, i32 6, !dbg !192 ; line:134 col:5
  %47 = getelementptr inbounds %struct.texture, %struct.texture* %0, i32 0, i32 6, !dbg !192 ; line:134 col:5
  %48 = load i32, i32* %47, !dbg !192 ; line:134 col:5
  store i32 %48, i32* %46, !dbg !192 ; line:134 col:5
  %49 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !193 ; line:154 col:5
  ret void, !dbg !193 ; line:154 col:5
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: nounwind
define void @rpsl_M_rpsl_triangle_Fn_main(%struct.texture* %backbuffer) #0 {
entry:
  call void @___rpsl_block_marker(i32 0, i32 0, i32 0, i32 2, i32 -1, i32 0, i32 -1)
  call void @llvm.dbg.declare(metadata %struct.texture* %backbuffer, metadata !194, metadata !149), !dbg !195 ; var:"backbuffer" !DIExpression() func:"main"
  %0 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !196 ; line:7 col:5
  call void @llvm.dbg.value(metadata <4 x float> <float 0.000000e+00, float 0x3FC99999A0000000, float 0x3FD99999A0000000, float 1.000000e+00>, i64 0, metadata !197, metadata !149), !dbg !198 ; var:"val" !DIExpression() func:"clear"
  call void @llvm.dbg.declare(metadata %struct.texture* %backbuffer, metadata !200, metadata !149) #0, !dbg !201 ; var:"t" !DIExpression() func:"clear"
  %1 = alloca i8*, i32 2, !dbg !202 ; line:294 col:12
  %2 = bitcast %struct.texture* %backbuffer to i8*, !dbg !202 ; line:294 col:12
  %3 = getelementptr i8*, i8** %1, i32 0, !dbg !202 ; line:294 col:12
  store i8* %2, i8** %3, !dbg !202 ; line:294 col:12
  %4 = alloca <4 x float>, !dbg !202 ; line:294 col:12
  store <4 x float> <float 0.000000e+00, float 0x3FC99999A0000000, float 0x3FD99999A0000000, float 1.000000e+00>, <4 x float>* %4, !dbg !202 ; line:294 col:12
  %5 = bitcast <4 x float>* %4 to i8*, !dbg !202 ; line:294 col:12
  %6 = getelementptr i8*, i8** %1, i32 1, !dbg !202 ; line:294 col:12
  store i8* %5, i8** %6, !dbg !202 ; line:294 col:12
  %7 = call i32 @___rpsl_node_call(i32 0, i32 2, i8** %1, i32 0, i32 0), !dbg !202 ; line:294 col:12
  %8 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !203 ; line:294 col:5
  %9 = alloca i8*, !dbg !204 ; line:8 col:5
  %10 = bitcast %struct.texture* %backbuffer to i8*, !dbg !204 ; line:8 col:5
  %11 = getelementptr i8*, i8** %9, i32 0, !dbg !204 ; line:8 col:5
  store i8* %10, i8** %11, !dbg !204 ; line:8 col:5
  %12 = call i32 @___rpsl_node_call(i32 1, i32 1, i8** %9, i32 0, i32 1), !dbg !204 ; line:8 col:5
  %13 = load i32, i32* getelementptr inbounds ([1 x i32], [1 x i32]* @dx.nothing.a, i32 0, i32 0), !dbg !205 ; line:9 col:1
  ret void, !dbg !205 ; line:9 col:1
}

; Function Attrs: nounwind
declare void @llvm.memset.p0i8.i32(i8* nocapture, i8, i32, i32, i1) #0

; Function Attrs: nounwind readnone
declare void @llvm.dbg.value(metadata, i64, metadata, metadata) #1

declare void @___rpsl_abort(i32)

declare i32 @___rpsl_node_call(i32, i32, i8**, i32, i32)

declare void @___rpsl_block_marker(i32, i32, i32, i32, i32, i32, i32)

; Function Attrs: noinline nounwind
define void @rpsl_M_rpsl_triangle_Fn_main_wrapper(i32, i8**, i32) #2 {
body:
  %3 = icmp eq i32 %0, 1, !dbg !206 ; line:4 col:0
  br i1 %3, label %trunk, label %err, !dbg !206 ; line:4 col:0

trunk:                                            ; preds = %body
  %4 = getelementptr i8*, i8** %1, i32 0, !dbg !206 ; line:4 col:0
  %5 = load i8*, i8** %4, !dbg !206 ; line:4 col:0
  %6 = bitcast i8* %5 to %struct.ResourceDesc*, !dbg !206 ; line:4 col:0
  %7 = alloca %struct.texture, !dbg !206 ; line:4 col:0
  %8 = alloca i32, !dbg !206 ; line:4 col:0
  store i32 0, i32* %8, !dbg !206 ; line:4 col:0
  %9 = and i32 %2, 1, !dbg !206 ; line:4 col:0
  %10 = trunc i32 %9 to i1, !dbg !206 ; line:4 col:0
  %11 = bitcast i8* %5 to %struct.texture*, !dbg !206 ; line:4 col:0
  br i1 %10, label %20, label %.preheader, !dbg !206 ; line:4 col:0

.preheader:                                       ; preds = %trunk
  br label %12, !dbg !206 ; line:4 col:0

err:                                              ; preds = %body
  call void @___rpsl_abort(i32 -3), !dbg !206 ; line:4 col:0
  ret void, !dbg !206 ; line:4 col:0

; <label>:12                                      ; preds = %15, %.preheader
  %13 = load i32, i32* %8, !dbg !206 ; line:4 col:0
  %14 = icmp slt i32 %13, 1, !dbg !206 ; line:4 col:0
  br i1 %14, label %15, label %.loopexit, !dbg !206 ; line:4 col:0

; <label>:15                                      ; preds = %12
  %16 = getelementptr %struct.ResourceDesc, %struct.ResourceDesc* %6, i32 %13, !dbg !206 ; line:4 col:0
  %17 = getelementptr %struct.texture, %struct.texture* %7, i32 %13, !dbg !206 ; line:4 col:0
  %18 = add i32 %13, 0, !dbg !206 ; line:4 col:0
  call fastcc void @"\01?make_default_texture_view_from_desc@@YA?AUtexture@@IUResourceDesc@@@Z"(%struct.texture* %17, i32 %18, %struct.ResourceDesc* %16), !dbg !206 ; line:4 col:0
  %19 = add i32 %13, 1, !dbg !206 ; line:4 col:0
  store i32 %19, i32* %8, !dbg !206 ; line:4 col:0
  br label %12, !dbg !206 ; line:4 col:0

.loopexit:                                        ; preds = %12
  br label %20, !dbg !206 ; line:4 col:0

; <label>:20                                      ; preds = %.loopexit, %trunk
  %21 = phi %struct.texture* [ %11, %trunk ], [ %7, %.loopexit ], !dbg !206 ; line:4 col:0
  call void @rpsl_M_rpsl_triangle_Fn_main(%struct.texture* %21), !dbg !206 ; line:4 col:0
  ret void
}

attributes #0 = { nounwind }
attributes #1 = { nounwind readnone }
attributes #2 = { noinline nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!101, !102}
!llvm.ident = !{!103}
!dx.source.contents = !{!104, !105}
!dx.source.defines = !{!2}
!dx.source.mainFileName = !{!106}
!dx.source.args = !{!107}
!dx.version = !{!108}
!dx.valver = !{!109}
!dx.shaderModel = !{!110}
!dx.typeAnnotations = !{!111, !135}
!dx.entryPoints = !{!144, !146}

!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus, file: !1, producer: "dxc(private) 1.7.0.3789 (rps-merge-dxc1_7_2212_squash, b8686eeb8)", isOptimized: false, runtimeVersion: 0, emissionKind: 1, enums: !2, retainedTypes: !3, subprograms: !20, globals: !93)
!1 = !DIFile(filename: "rpsl_triangle.rpsl", directory: "")
!2 = !{}
!3 = !{!4, !6, !18}
!4 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint64_t", file: !1, line: 165, baseType: !5)
!5 = !DIBasicType(name: "uint64_t", size: 64, align: 64, encoding: DW_ATE_unsigned)
!6 = !DIDerivedType(tag: DW_TAG_typedef, name: "float4", file: !1, line: 7, baseType: !7)
!7 = !DICompositeType(tag: DW_TAG_class_type, name: "vector<float, 4>", file: !1, line: 7, size: 128, align: 32, elements: !8, templateParams: !14)
!8 = !{!9, !11, !12, !13}
!9 = !DIDerivedType(tag: DW_TAG_member, name: "x", scope: !7, file: !1, line: 7, baseType: !10, size: 32, align: 32, flags: DIFlagPublic)
!10 = !DIBasicType(name: "float", size: 32, align: 32, encoding: DW_ATE_float)
!11 = !DIDerivedType(tag: DW_TAG_member, name: "y", scope: !7, file: !1, line: 7, baseType: !10, size: 32, align: 32, offset: 32, flags: DIFlagPublic)
!12 = !DIDerivedType(tag: DW_TAG_member, name: "z", scope: !7, file: !1, line: 7, baseType: !10, size: 32, align: 32, offset: 64, flags: DIFlagPublic)
!13 = !DIDerivedType(tag: DW_TAG_member, name: "w", scope: !7, file: !1, line: 7, baseType: !10, size: 32, align: 32, offset: 96, flags: DIFlagPublic)
!14 = !{!15, !16}
!15 = !DITemplateTypeParameter(name: "element", type: !10)
!16 = !DITemplateValueParameter(name: "element_count", type: !17, value: i32 4)
!17 = !DIBasicType(name: "int", size: 32, align: 32, encoding: DW_ATE_signed)
!18 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint16_t", file: !1, line: 128, baseType: !19)
!19 = !DIBasicType(name: "uint16_t", size: 16, align: 16, encoding: DW_ATE_unsigned)
!20 = !{!21, !62, !77, !80, !84, !87}
!21 = !DISubprogram(name: "make_default_texture_view_from_desc", linkageName: "\01?make_default_texture_view_from_desc@@YA?AUtexture@@IUResourceDesc@@@Z", scope: !22, file: !22, line: 152, type: !23, isLocal: false, isDefinition: true, scopeLine: 153, flags: DIFlagPrototyped, isOptimized: false, function: void (%struct.texture*, i32, %struct.ResourceDesc*)* @"\01?make_default_texture_view_from_desc@@YA?AUtexture@@IUResourceDesc@@@Z")
!22 = !DIFile(filename: "./___rpsl_builtin_header_.rpsl", directory: "")
!23 = !DISubroutineType(types: !24)
!24 = !{!25, !50, !51}
!25 = !DICompositeType(tag: DW_TAG_structure_type, name: "texture", file: !1, size: 288, align: 32, elements: !26)
!26 = !{!27, !29, !30, !31, !32, !39, !40, !41, !44, !47}
!27 = !DIDerivedType(tag: DW_TAG_member, name: "Resource", scope: !25, file: !1, baseType: !28, size: 32, align: 32)
!28 = !DIBasicType(name: "unsigned int", size: 32, align: 32, encoding: DW_ATE_unsigned)
!29 = !DIDerivedType(tag: DW_TAG_member, name: "ViewFormat", scope: !25, file: !1, baseType: !28, size: 32, align: 32, offset: 32)
!30 = !DIDerivedType(tag: DW_TAG_member, name: "TemporalLayer", scope: !25, file: !1, baseType: !28, size: 32, align: 32, offset: 64)
!31 = !DIDerivedType(tag: DW_TAG_member, name: "Flags", scope: !25, file: !1, baseType: !28, size: 32, align: 32, offset: 96)
!32 = !DIDerivedType(tag: DW_TAG_member, name: "SubresourceRange", scope: !25, file: !1, baseType: !33, size: 96, align: 32, offset: 128)
!33 = !DICompositeType(tag: DW_TAG_structure_type, name: "SubresourceRange", file: !1, size: 96, align: 32, elements: !34)
!34 = !{!35, !36, !37, !38}
!35 = !DIDerivedType(tag: DW_TAG_member, name: "base_mip_level", scope: !33, file: !1, baseType: !19, size: 16, align: 16)
!36 = !DIDerivedType(tag: DW_TAG_member, name: "mip_level_count", scope: !33, file: !1, baseType: !19, size: 16, align: 16, offset: 16)
!37 = !DIDerivedType(tag: DW_TAG_member, name: "base_array_layer", scope: !33, file: !1, baseType: !28, size: 32, align: 32, offset: 32)
!38 = !DIDerivedType(tag: DW_TAG_member, name: "array_layer_count", scope: !33, file: !1, baseType: !28, size: 32, align: 32, offset: 64)
!39 = !DIDerivedType(tag: DW_TAG_member, name: "MinLodClamp", scope: !25, file: !1, baseType: !10, size: 32, align: 32, offset: 224)
!40 = !DIDerivedType(tag: DW_TAG_member, name: "ComponentMapping", scope: !25, file: !1, baseType: !28, size: 32, align: 32, offset: 256)
!41 = !DISubprogram(name: "create1D", linkageName: "\01?create1D@texture@@SA?AU1@IIIIII@Z", scope: !25, file: !1, type: !42, isLocal: false, isDefinition: false, flags: DIFlagPrototyped, isOptimized: false)
!42 = !DISubroutineType(types: !43)
!43 = !{!25, !28, !28, !28, !28, !28, !28}
!44 = !DISubprogram(name: "create2D", linkageName: "\01?create2D@texture@@SA?AU1@IIIIIIIII@Z", scope: !25, file: !1, type: !45, isLocal: false, isDefinition: false, flags: DIFlagPrototyped, isOptimized: false)
!45 = !DISubroutineType(types: !46)
!46 = !{!25, !28, !28, !28, !28, !28, !28, !28, !28, !28}
!47 = !DISubprogram(name: "create3D", linkageName: "\01?create3D@texture@@SA?AU1@IIIIIII@Z", scope: !25, file: !1, type: !48, isLocal: false, isDefinition: false, flags: DIFlagPrototyped, isOptimized: false)
!48 = !DISubroutineType(types: !49)
!49 = !{!25, !28, !28, !28, !28, !28, !28, !28}
!50 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint", file: !1, baseType: !28)
!51 = !DICompositeType(tag: DW_TAG_structure_type, name: "ResourceDesc", file: !1, size: 288, align: 32, elements: !52)
!52 = !{!53, !54, !55, !56, !57, !58, !59, !60, !61}
!53 = !DIDerivedType(tag: DW_TAG_member, name: "Type", scope: !51, file: !1, baseType: !28, size: 32, align: 32)
!54 = !DIDerivedType(tag: DW_TAG_member, name: "TemporalLayers", scope: !51, file: !1, baseType: !28, size: 32, align: 32, offset: 32)
!55 = !DIDerivedType(tag: DW_TAG_member, name: "Flags", scope: !51, file: !1, baseType: !28, size: 32, align: 32, offset: 64)
!56 = !DIDerivedType(tag: DW_TAG_member, name: "Width", scope: !51, file: !1, baseType: !28, size: 32, align: 32, offset: 96)
!57 = !DIDerivedType(tag: DW_TAG_member, name: "Height", scope: !51, file: !1, baseType: !28, size: 32, align: 32, offset: 128)
!58 = !DIDerivedType(tag: DW_TAG_member, name: "DepthOrArraySize", scope: !51, file: !1, baseType: !28, size: 32, align: 32, offset: 160)
!59 = !DIDerivedType(tag: DW_TAG_member, name: "MipLevels", scope: !51, file: !1, baseType: !28, size: 32, align: 32, offset: 192)
!60 = !DIDerivedType(tag: DW_TAG_member, name: "Format", scope: !51, file: !1, baseType: !28, size: 32, align: 32, offset: 224)
!61 = !DIDerivedType(tag: DW_TAG_member, name: "SampleCount", scope: !51, file: !1, baseType: !28, size: 32, align: 32, offset: 256)
!62 = !DISubprogram(name: "make_default_buffer_view_from_desc", linkageName: "\01?make_default_buffer_view_from_desc@@YA?AUbuffer@@IUResourceDesc@@@Z", scope: !22, file: !22, line: 161, type: !63, isLocal: false, isDefinition: true, scopeLine: 162, flags: DIFlagPrototyped, isOptimized: false)
!63 = !DISubroutineType(types: !64)
!64 = !{!65, !50, !51}
!65 = !DICompositeType(tag: DW_TAG_structure_type, name: "buffer", file: !1, line: 159, size: 320, align: 64, elements: !66)
!66 = !{!67, !68, !69, !70, !71, !72, !73, !74}
!67 = !DIDerivedType(tag: DW_TAG_member, name: "Resource", scope: !65, file: !1, line: 159, baseType: !28, size: 32, align: 32)
!68 = !DIDerivedType(tag: DW_TAG_member, name: "ViewFormat", scope: !65, file: !1, line: 159, baseType: !28, size: 32, align: 32, offset: 32)
!69 = !DIDerivedType(tag: DW_TAG_member, name: "TemporalLayer", scope: !65, file: !1, line: 159, baseType: !28, size: 32, align: 32, offset: 64)
!70 = !DIDerivedType(tag: DW_TAG_member, name: "Flags", scope: !65, file: !1, line: 159, baseType: !28, size: 32, align: 32, offset: 96)
!71 = !DIDerivedType(tag: DW_TAG_member, name: "Offset", scope: !65, file: !1, line: 159, baseType: !5, size: 64, align: 64, offset: 128)
!72 = !DIDerivedType(tag: DW_TAG_member, name: "SizeInBytes", scope: !65, file: !1, line: 159, baseType: !5, size: 64, align: 64, offset: 192)
!73 = !DIDerivedType(tag: DW_TAG_member, name: "Stride", scope: !65, file: !1, line: 159, baseType: !28, size: 32, align: 32, offset: 256)
!74 = !DISubprogram(name: "create", linkageName: "\01?create@buffer@@SA?AU1@_KII@Z", scope: !65, file: !1, line: 159, type: !75, isLocal: false, isDefinition: false, scopeLine: 159, flags: DIFlagPrototyped, isOptimized: false)
!75 = !DISubroutineType(types: !76)
!76 = !{!65, !5, !28, !28}
!77 = !DISubprogram(name: "main", linkageName: "\01?main@@YAXUtexture@@@Z", scope: !1, file: !1, line: 4, type: !78, isLocal: false, isDefinition: true, scopeLine: 5, flags: DIFlagPrototyped, isOptimized: false, function: void (%struct.texture*)* @rpsl_M_rpsl_triangle_Fn_main)
!78 = !DISubroutineType(types: !79)
!79 = !{null, !25}
!80 = !DISubprogram(name: "make_default_texture_view", linkageName: "\01?make_default_texture_view@@YA?AUtexture@@IIII@Z", scope: !22, file: !22, line: 119, type: !81, isLocal: false, isDefinition: true, scopeLine: 120, flags: DIFlagPrototyped, isOptimized: false)
!81 = !DISubroutineType(types: !82)
!82 = !{!25, !50, !83, !50, !50}
!83 = !DIDerivedType(tag: DW_TAG_typedef, name: "RPS_FORMAT", file: !1, line: 9, baseType: !28)
!84 = !DISubprogram(name: "make_default_buffer_view", linkageName: "\01?make_default_buffer_view@@YA?AUbuffer@@I_K@Z", scope: !22, file: !22, line: 137, type: !85, isLocal: false, isDefinition: true, scopeLine: 138, flags: DIFlagPrototyped, isOptimized: false)
!85 = !DISubroutineType(types: !86)
!86 = !{!65, !50, !4}
!87 = !DISubprogram(name: "clear", linkageName: "\01?clear@@YA?AUnodeidentifier@@Utexture@@V?$vector@M$03@@@Z", scope: !22, file: !22, line: 292, type: !88, isLocal: false, isDefinition: true, scopeLine: 293, flags: DIFlagPrototyped, isOptimized: false)
!88 = !DISubroutineType(types: !89)
!89 = !{!90, !25, !6}
!90 = !DICompositeType(tag: DW_TAG_structure_type, name: "nodeidentifier", file: !22, line: 2, size: 32, align: 32, elements: !91)
!91 = !{!92}
!92 = !DIDerivedType(tag: DW_TAG_member, name: "unused", scope: !90, file: !22, line: 2, baseType: !50, size: 32, align: 32)
!93 = !{!94, !96, !97}
!94 = !DIGlobalVariable(name: "RPS_RESOURCE_TEX3D", scope: !0, file: !1, line: 157, type: !95, isLocal: true, isDefinition: true, variable: i32 4)
!95 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !28)
!96 = !DIGlobalVariable(name: "RPS_FORMAT_UNKNOWN", scope: !0, file: !1, line: 124, type: !95, isLocal: true, isDefinition: true, variable: i32 0)
!97 = !DIGlobalVariable(name: "___rpsl_pso_tmp", scope: !0, file: !22, line: 361, type: !98, isLocal: true, isDefinition: true)
!98 = !DICompositeType(tag: DW_TAG_structure_type, name: "Pipeline", file: !1, line: 361, size: 32, align: 32, elements: !99)
!99 = !{!100}
!100 = !DIDerivedType(tag: DW_TAG_member, name: "h", scope: !98, file: !1, line: 361, baseType: !17, size: 32, align: 32, flags: DIFlagPrivate)
!101 = !{i32 2, !"Dwarf Version", i32 4}
!102 = !{i32 2, !"Debug Info Version", i32 3}
!103 = !{!"dxc(private) 1.7.0.3789 (rps-merge-dxc1_7_2212_squash, b8686eeb8)"}
!104 = !{!"rpsl_triangle.rpsl", !"#include \22___rpsl_builtin_header_.rpsl\22\0Agraphics node Triangle([readwrite(rendertarget)] texture renderTarget : SV_Target0);\0D\0A\0D\0Aexport void main([readonly(present)] texture backbuffer)\0D\0A{\0D\0A    // clear and then render geometry to backbuffer\0D\0A    clear(backbuffer, float4(0.0, 0.2, 0.4, 1.0));\0D\0A    Triangle(backbuffer);\0D\0A}"}
!105 = !{!".\5C___rpsl_builtin_header_.rpsl", !"\0Astruct nodeidentifier { uint unused; };\0A#define node nodeidentifier\0A\0Auint ___rpsl_asyncmarker();\0A#define async ___rpsl_asyncmarker();\0A\0Avoid ___rpsl_barrier();\0A#define sch_barrier ___rpsl_barrier\0A\0Avoid ___rpsl_subgraph_begin(uint flags, uint nameOffs, uint nameLen);\0Avoid ___rpsl_subgraph_end();\0A\0Avoid ___rpsl_abort(int errorCode);\0A\0A#define abort ___rpsl_abort\0A\0Atypedef int RpsBool;\0Atypedef RPS_FORMAT RpsFormat;\0A\0A// Syntax sugars\0A#define rtv         [readwrite(rendertarget)] texture\0A#define discard_rtv [writeonly(rendertarget)] texture\0A#define srv         [readonly(ps, cs)] texture\0A#define srv_buf     [readonly(ps, cs)] buffer\0A#define ps_srv      [readonly(ps)] texture\0A#define ps_srv_buf  [readonly(ps)] buffer\0A#define dsv         [readwrite(depth, stencil)] texture\0A#define uav         [readwrite(ps, cs)] texture\0A#define uav_buf     [readwrite(ps, cs)] buffer\0A\0Atexture ___rpsl_set_resource_name(texture h, uint nameOffset, uint nameLength);\0Abuffer ___rpsl_set_resource_name(buffer h, uint nameOffset, uint nameLength);\0A\0Astruct RpsViewport\0A{\0A    float x, y, width, height, minZ, maxZ;\0A};\0A\0Ainline RpsViewport viewport(float x, float y, float width, float height, float minZ = 0.0f, float maxZ = 1.0f)\0A{\0A    RpsViewport result = { x, y, width, height, minZ, maxZ };\0A    return result;\0A}\0A\0Ainline RpsViewport viewport(float width, float height)\0A{\0A    RpsViewport result = { 0.0f, 0.0f, width, height, 0.0f, 1.0f };\0A    return result;\0A}\0A\0Ainline ResourceDesc     describe_resource      ( texture t ) { return t.desc(); }\0Ainline ResourceDesc     describe_resource      ( buffer b  ) { return b.desc(); }\0Ainline ResourceDesc     describe_texture       ( texture t ) { return t.desc(); }\0Ainline ResourceDesc     describe_buffer        ( buffer b  ) { return b.desc(); }\0A\0Auint             ___rpsl_create_resource( RPS_RESOURCE_TYPE  type,\0A                                          RPS_RESOURCE_FLAGS flags,\0A                                          RPS_FORMAT         format,\0A                                          uint               width,\0A                                          uint               height,\0A                                          uint               depthOrArraySize,\0A                                          uint               mipLevels,\0A                                          uint               sampleCount,\0A                                          uint               sampleQuality,\0A                                          uint               temporalLayers,\0A                                          uint               id = 0xFFFFFFFFu );\0A\0A// Built in nodes\0Atemplate<uint MaxRects>\0Agraphics node clear_color_regions( [readwrite(rendertarget, clear)] texture t, float4 data : SV_ClearColor, uint numRects, int4 rects[MaxRects] );\0Atemplate<uint MaxRects>\0Agraphics node clear_depth_stencil_regions( [readwrite(depth, stencil, clear)] texture t, RPS_CLEAR_FLAGS option, float d : SV_ClearDepth, uint s : SV_ClearStencil, uint numRects, int4 rects[MaxRects] );\0Atemplate<uint MaxRects>\0Acompute  node clear_texture_regions( [readwrite(clear)] texture t, uint4 data : SV_ClearColor, uint numRects, int4 rects[MaxRects] );\0A\0Agraphics node    clear_color            ( [writeonly(rendertarget, clear)] texture t, float4 data : SV_ClearColor );\0Agraphics node    clear_depth_stencil    ( [writeonly(depth, stencil, clear)] texture t, RPS_CLEAR_FLAGS option, float d : SV_ClearDepth, uint s : SV_ClearStencil );\0Acompute  node    clear_texture          ( [writeonly(clear)] texture t, uint4 data : SV_ClearColor );\0Acopy     node    clear_buffer           ( [writeonly(clear)] buffer b, uint4 data );\0Acopy     node    copy_texture           ( [readwrite(copy)] texture dst, uint3 dstOffset, [readonly(copy)] texture src, uint3 srcOffset, uint3 extent );\0Acopy     node    copy_buffer            ( [readwrite(copy)] buffer dst, uint64_t dstOffset, [readonly(copy)] buffer src, uint64_t srcOffset, uint64_t size );\0Acopy     node    copy_texture_to_buffer ( [readwrite(copy)] buffer dst, uint64_t dstByteOffset, uint rowPitch, uint3 bufferImageSize, uint3 dstOffset, [readonly(copy)] texture src, uint3 srcOffset, uint3 extent );\0Acopy     node    copy_buffer_to_texture ( [readwrite(copy)] texture dst, uint3 dstOffset, [readonly(copy)] buffer src, uint64_t srcByteOffset, uint rowPitch, uint3 bufferImageSize, uint3 srcOffset, uint3 extent );\0Agraphics node    resolve                ( [readwrite(resolve)] texture dst, uint2 dstOffset, [readonly(resolve)] texture src, uint2 srcOffset, uint2 extent, RPS_RESOLVE_MODE resolveMode );\0A\0Agraphics node    draw                   ( uint vertexCountPerInstance, uint instanceCount, uint startVertexLocation, uint startInstanceLocation );\0Agraphics node    draw_indexed           ( uint indexCountPerInstance, uint instanceCount, uint startIndexLocation, int baseVertexLocation, uint startInstanceLocation );\0Acompute  node    dispatch               ( uint3 numGroups );\0Acompute  node    dispatch_threads       ( uint3 numThreads );\0Agraphics node    draw_indirect          ( buffer args, uint64_t offset, uint32_t drawCount, uint32_t stride );\0Agraphics node    draw_indexed_indirect  ( buffer args, uint64_t offset, uint32_t drawCount, uint32_t stride );\0A\0Avoid             bind                   ( Pipeline pso );\0A\0Ainline uint ___rpsl_canonicalize_mip_level(uint inMipLevel, uint width, uint height = 1, uint depth = 1, uint sampleCount = 1)\0A{\0A    if (sampleCount > 1)\0A    {\0A        return 1;\0A    }\0A\0A    uint32_t w    = width;\0A    uint32_t h    = height;\0A    uint32_t d    = depth;\0A    uint32_t mips = 1;\0A\0A    while ((w > 1) || (h > 1) || (d > 1))\0A    {\0A        mips++;\0A        w = w >> 1;\0A        h = h >> 1;\0A        d = d >> 1;\0A    }\0A\0A    return (inMipLevel == 0) ? mips : min(inMipLevel, mips);\0A}\0A\0Ainline texture make_default_texture_view( uint resourceHdl, RPS_FORMAT format, uint arraySlices, uint numMips )\0A{\0A    texture result;\0A\0A    result.Resource = resourceHdl;\0A    result.ViewFormat = RPS_FORMAT_UNKNOWN;\0A    result.TemporalLayer = 0;\0A    result.Flags = 0;\0A    result.SubresourceRange.base_mip_level = 0;\0A    result.SubresourceRange.mip_level_count = uint16_t(numMips);\0A    result.SubresourceRange.base_array_layer = 0;\0A    result.SubresourceRange.array_layer_count = arraySlices;\0A    result.MinLodClamp = 0.0f;\0A    result.ComponentMapping = 0x3020100; // Default channel mapping\0A\0A    return result;\0A}\0A\0Ainline buffer make_default_buffer_view( uint resourceHdl, uint64_t size )\0A{\0A    buffer result;\0A\0A    result.Resource = resourceHdl;\0A    result.ViewFormat = 0;\0A    result.TemporalLayer = 0;\0A    result.Flags = 0;\0A    result.Offset = 0;\0A    result.SizeInBytes = size;\0A    result.Stride = 0;\0A\0A    return result;\0A}\0A\0Atexture make_default_texture_view_from_desc( uint resourceHdl, ResourceDesc desc )\0A{\0A    return make_default_texture_view(\0A        resourceHdl,\0A        desc.Format,\0A        (desc.Type == RPS_RESOURCE_TEX3D) ? 1 : desc.DepthOrArraySize,\0A        desc.MipLevels);\0A}\0A\0Abuffer make_default_buffer_view_from_desc( uint resourceHdl, ResourceDesc desc )\0A{\0A    return make_default_buffer_view(\0A        resourceHdl,\0A        (uint64_t(desc.Height) << 32u) | uint64_t(desc.Width));\0A}\0A\0Ainline texture create_texture( ResourceDesc desc )\0A{\0A    // TODO: Report error if type is not texture.\0A\0A    desc.MipLevels = ___rpsl_canonicalize_mip_level(desc.MipLevels,\0A                                                    desc.Width,\0A                                                    (desc.Type != RPS_RESOURCE_TEX1D) ? desc.Height : 1,\0A                                                    (desc.Type == RPS_RESOURCE_TEX3D) ? desc.DepthOrArraySize : 1,\0A                                                    desc.SampleCount);\0A\0A    uint resourceHdl = ___rpsl_create_resource( desc.Type, desc.Flags, desc.Format, desc.Width, desc.Height, desc.DepthOrArraySize, desc.MipLevels, desc.SampleCount, 0, desc.TemporalLayers );\0A\0A    uint arraySize = (desc.Type != RPS_RESOURCE_TEX3D) ? desc.DepthOrArraySize : 1;\0A\0A    return make_default_texture_view( resourceHdl, desc.Format, arraySize, desc.MipLevels );\0A}\0A\0Ainline buffer create_buffer( ResourceDesc desc )\0A{\0A    // TODO: Report error if type is not buffer.\0A\0A    uint resourceHdl = ___rpsl_create_resource( desc.Type, desc.Flags, desc.Format, desc.Width, desc.Height, desc.DepthOrArraySize, desc.MipLevels, desc.SampleCount, 0, desc.TemporalLayers );\0A\0A    uint64_t byteWidth = ((uint64_t)(desc.Height) << 32ULL) | ((uint64_t)(desc.Width));\0A\0A    return make_default_buffer_view( resourceHdl, byteWidth );\0A}\0A\0Ainline texture create_tex1d(\0A    RPS_FORMAT format,\0A    uint width,\0A    uint numMips = 1,\0A    uint arraySlices = 1,\0A    uint numTemporalLayers = 1,\0A    RPS_RESOURCE_FLAGS flags = RPS_RESOURCE_FLAG_NONE )\0A{\0A    const RPS_RESOURCE_TYPE type = RPS_RESOURCE_TEX1D;\0A    const uint height = 1;\0A    const uint sampleCount = 1;\0A    const uint sampleQuality = 0;\0A\0A    numMips = ___rpsl_canonicalize_mip_level(numMips, width, height, 1, sampleCount);\0A\0A    uint resourceHdl = ___rpsl_create_resource( type, flags, format, width, height, arraySlices, numMips, sampleCount, sampleQuality, numTemporalLayers );\0A\0A    return make_default_texture_view( resourceHdl, format, arraySlices, numMips );\0A}\0A\0Ainline texture create_tex2d(\0A    RPS_FORMAT format,\0A    uint width,\0A    uint height,\0A    uint numMips = 1,\0A    uint arraySlices = 1,\0A    uint numTemporalLayers = 1,\0A    uint sampleCount = 1,\0A    uint sampleQuality = 0,\0A    RPS_RESOURCE_FLAGS flags = RPS_RESOURCE_FLAG_NONE )\0A{\0A    const RPS_RESOURCE_TYPE type = RPS_RESOURCE_TEX2D;\0A\0A    numMips = ___rpsl_canonicalize_mip_level(numMips, width, height, 1, sampleCount);\0A\0A    const uint resourceHdl = ___rpsl_create_resource( type, flags, format, width, height, arraySlices, numMips, sampleCount, sampleQuality, numTemporalLayers );\0A\0A    return make_default_texture_view( resourceHdl, format, arraySlices, numMips );\0A}\0A\0Ainline texture create_tex3d(\0A    RPS_FORMAT format,\0A    uint width,\0A    uint height,\0A    uint depth,\0A    uint numMips = 1,\0A    uint numTemporalLayers = 1,\0A    RPS_RESOURCE_FLAGS flags = RPS_RESOURCE_FLAG_NONE )\0A{\0A    const RPS_RESOURCE_TYPE type = RPS_RESOURCE_TEX3D;\0A    const uint sampleCount = 1;\0A    const uint sampleQuality = 0;\0A    const uint arraySlices = 1;\0A\0A    numMips = ___rpsl_canonicalize_mip_level(numMips, width, height, depth, sampleCount);\0A\0A    uint resourceHdl = ___rpsl_create_resource( type, flags, format, width, height, depth, numMips, sampleCount, sampleQuality, numTemporalLayers );\0A\0A    return make_default_texture_view( resourceHdl, format, arraySlices, numMips );\0A}\0A\0Ainline buffer create_buffer( uint64_t width, uint numTemporalLayers = 1, RPS_RESOURCE_FLAGS flags = RPS_RESOURCE_FLAG_NONE )\0A{\0A    uint resourceHdl = ___rpsl_create_resource( RPS_RESOURCE_BUFFER,\0A                                              flags,\0A                                              RPS_FORMAT_UNKNOWN,\0A                                              (uint)width,\0A                                              (uint)(width >> 32ULL),\0A                                              1, 1, 1, 0, numTemporalLayers );\0A\0A    return make_default_buffer_view( resourceHdl, width );\0A}\0A\0Ainline texture create_texture_view(\0A    texture r,\0A    uint baseMip = 0,\0A    uint mipLevels = 1,\0A    uint baseArraySlice = 0,\0A    uint numArraySlices = 1,\0A    uint temporalLayer = 0,\0A    RPS_FORMAT format = RPS_FORMAT_UNKNOWN )\0A{\0A    return r.temporal(temporalLayer).mips(baseMip, mipLevels).array(baseArraySlice, numArraySlices).format(format);\0A}\0A\0Ainline buffer create_buffer_view(\0A    buffer r,\0A    uint64_t offset = 0,\0A    uint64_t sizeInBytes = 0,\0A    uint temporalLayer = 0,\0A    RPS_FORMAT format = RPS_FORMAT_UNKNOWN,\0A    uint structureStride = 0 )\0A{\0A    return r.temporal(temporalLayer).bytes(offset, sizeInBytes).format(format).stride(structureStride);\0A}\0A\0Ainline node clear( texture t, float4 val )\0A{\0A    return clear_color( t, val );\0A}\0A\0Ainline node clear( texture t, uint4 val )\0A{\0A    return clear_color( t, float4(val) );\0A}\0A\0Ainline node clear( texture t, float depth, uint stencil )\0A{\0A    return clear_depth_stencil( t, RPS_CLEAR_DEPTHSTENCIL, depth, stencil );\0A}\0A\0Ainline node clear_depth( texture t, float depth )\0A{\0A    return clear_depth_stencil( t, RPS_CLEAR_DEPTH, depth, 0 );\0A}\0A\0Ainline node clear_stencil( texture t, uint stencil )\0A{\0A    return clear_depth_stencil( t, RPS_CLEAR_STENCIL, 0.0f, stencil );\0A}\0A\0Atemplate<uint MaxRects>\0Ainline node clear_depth_regions( texture t, float depth, int4 rects[MaxRects] )\0A{\0A    return clear_depth_stencil_regions( t, RPS_CLEAR_DEPTH, depth, 0, MaxRects, rects );\0A}\0A\0Atemplate<uint MaxRects>\0Ainline node clear_stencil_regions( texture t, uint stencil, int4 rects[MaxRects] )\0A{\0A    return clear_depth_stencil_regions( t, RPS_CLEAR_STENCIL, 0.0f, stencil, MaxRects, rects );\0A}\0A\0Ainline node clear_uav( texture t, float4 val )\0A{\0A    return clear_texture( t, asuint(val) );\0A}\0A\0Ainline node clear_uav( texture t, uint4 val )\0A{\0A    return clear_texture( t, val );\0A}\0A\0Ainline node clear( buffer b, float4 val )\0A{\0A    return clear_buffer( b, asuint(val) );\0A}\0A\0Ainline node clear( buffer b, uint4 val )\0A{\0A    return clear_buffer( b, val );\0A}\0A\0Ainline node copy_texture( texture dst, texture src )\0A{\0A    return copy_texture( dst, uint3(0, 0, 0), src, uint3(0, 0, 0), uint3(0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU) );\0A}\0A\0Ainline node copy_buffer( buffer dst, buffer src )\0A{\0A    return copy_buffer( dst, 0, src, 0, 0xFFFFFFFFFFFFFFFFULL );\0A}\0A\0A// Experimental Pipeline states\0A\0Astatic Pipeline ___rpsl_pso_tmp;\0A\0Atemplate<typename T>\0Avoid ___rpsl_pso_modify_field(Pipeline p, uint i, T val);\0A\0Aenum RPS_DEPTH_WRITE_MASK {\0A  RPS_DEPTH_WRITE_MASK_ZERO,\0A  RPS_DEPTH_WRITE_MASK_ALL\0A};\0A\0Aenum RPS_COMPARISON_FUNC {\0A  RPS_COMPARISON_FUNC_NEVER,\0A  RPS_COMPARISON_FUNC_LESS,\0A  RPS_COMPARISON_FUNC_EQUAL,\0A  RPS_COMPARISON_FUNC_LESS_EQUAL,\0A  RPS_COMPARISON_FUNC_GREATER,\0A  RPS_COMPARISON_FUNC_NOT_EQUAL,\0A  RPS_COMPARISON_FUNC_GREATER_EQUAL,\0A  RPS_COMPARISON_FUNC_ALWAYS\0A};\0A\0Aenum RPS_STENCIL_OP {\0A  RPS_STENCIL_OP_KEEP,\0A  RPS_STENCIL_OP_ZERO,\0A  RPS_STENCIL_OP_REPLACE,\0A  RPS_STENCIL_OP_INCR_SAT,\0A  RPS_STENCIL_OP_DECR_SAT,\0A  RPS_STENCIL_OP_INVERT,\0A  RPS_STENCIL_OP_INCR,\0A  RPS_STENCIL_OP_DECR\0A};\0A\0Astruct RpsStencilOpDesc {\0A  RPS_STENCIL_OP      StencilFailOp;\0A  RPS_STENCIL_OP      StencilDepthFailOp;\0A  RPS_STENCIL_OP      StencilPassOp;\0A  RPS_COMPARISON_FUNC StencilFunc;\0A};\0A\0Astruct RpsDepthStencilDesc {\0A  RpsBool              DepthEnable;\0A  RPS_DEPTH_WRITE_MASK DepthWriteMask;\0A  RPS_COMPARISON_FUNC  DepthFunc;\0A  RpsBool              StencilEnable;\0A  uint                 StencilReadMask;\0A  uint                 StencilWriteMask;\0A  RpsStencilOpDesc     FrontFace;\0A  RpsStencilOpDesc     BackFace;\0A};\0A\0Aenum RPS_BLEND {\0A  RPS_BLEND_ZERO = 0,\0A  RPS_BLEND_ONE,\0A  RPS_BLEND_SRC_COLOR,\0A  RPS_BLEND_INV_SRC_COLOR,\0A  RPS_BLEND_SRC_ALPHA,\0A  RPS_BLEND_INV_SRC_ALPHA,\0A  RPS_BLEND_DEST_ALPHA,\0A  RPS_BLEND_INV_DEST_ALPHA,\0A  RPS_BLEND_DEST_COLOR,\0A  RPS_BLEND_INV_DEST_COLOR,\0A  RPS_BLEND_SRC_ALPHA_SAT,\0A  RPS_BLEND_BLEND_FACTOR,\0A  RPS_BLEND_INV_BLEND_FACTOR,\0A  RPS_BLEND_SRC1_COLOR,\0A  RPS_BLEND_INV_SRC1_COLOR,\0A  RPS_BLEND_SRC1_ALPHA,\0A  RPS_BLEND_INV_SRC1_ALPHA\0A};\0A\0Aenum RPS_BLEND_OP {\0A  RPS_BLEND_OP_ADD,\0A  RPS_BLEND_OP_SUBTRACT,\0A  RPS_BLEND_OP_REV_SUBTRACT,\0A  RPS_BLEND_OP_MIN,\0A  RPS_BLEND_OP_MAX\0A};\0A\0Aenum RPS_LOGIC_OP {\0A  RPS_LOGIC_OP_CLEAR = 0,\0A  RPS_LOGIC_OP_SET,\0A  RPS_LOGIC_OP_COPY,\0A  RPS_LOGIC_OP_COPY_INVERTED,\0A  RPS_LOGIC_OP_NOOP,\0A  RPS_LOGIC_OP_INVERT,\0A  RPS_LOGIC_OP_AND,\0A  RPS_LOGIC_OP_NAND,\0A  RPS_LOGIC_OP_OR,\0A  RPS_LOGIC_OP_NOR,\0A  RPS_LOGIC_OP_XOR,\0A  RPS_LOGIC_OP_EQUIV,\0A  RPS_LOGIC_OP_AND_REVERSE,\0A  RPS_LOGIC_OP_AND_INVERTED,\0A  RPS_LOGIC_OP_OR_REVERSE,\0A  RPS_LOGIC_OP_OR_INVERTED\0A};\0A\0Astruct RpsRenderTargetBlendDesc {\0A  RpsBool      BlendEnable;\0A  RpsBool      LogicOpEnable;\0A  RPS_BLEND    SrcBlend;\0A  RPS_BLEND    DestBlend;\0A  RPS_BLEND_OP BlendOp;\0A  RPS_BLEND    SrcBlendAlpha;\0A  RPS_BLEND    DestBlendAlpha;\0A  RPS_BLEND_OP BlendOpAlpha;\0A  RPS_LOGIC_OP LogicOp;\0A  uint         RenderTargetWriteMask;\0A};\0A\0Astruct RpsBlendDesc {\0A  RpsBool AlphaToCoverageEnable;\0A  RpsBool IndependentBlendEnable;\0A};\0A\0A#define per_vertex \22per_vertex\22\0A#define per_instance \22per_instance\22\0A\0ARpsVertexLayout vertex_layout(uint stride);\0A"}
!106 = !{!"rpsl_triangle.rpsl"}
!107 = !{!"-T", !"rps_6_2", !"-Vd", !"-default-linkage", !"external", !"-res_may_alias", !"-Zi", !"-Qembed_debug", !"-enable-16bit-types", !"-O0", !"-Wno-for-redefinition", !"-Wno-comma-in-init", !"-rps-module-name", !"rpsl_triangle", !"-HV", !"2021"}
!108 = !{i32 1, i32 2}
!109 = !{i32 0, i32 0}
!110 = !{!"rps", i32 6, i32 2}
!111 = !{i32 0, %struct.texture undef, !112, %struct.SubresourceRange undef, !120, %struct.ResourceDesc undef, !125}
!112 = !{i32 36, !113, !114, !115, !116, !117, !118, !119}
!113 = !{i32 6, !"Resource", i32 3, i32 0, i32 7, i32 5}
!114 = !{i32 6, !"ViewFormat", i32 3, i32 4, i32 7, i32 5}
!115 = !{i32 6, !"TemporalLayer", i32 3, i32 8, i32 7, i32 5}
!116 = !{i32 6, !"Flags", i32 3, i32 12, i32 7, i32 5}
!117 = !{i32 6, !"SubresourceRange", i32 3, i32 16}
!118 = !{i32 6, !"MinLodClamp", i32 3, i32 28, i32 7, i32 9}
!119 = !{i32 6, !"ComponentMapping", i32 3, i32 32, i32 7, i32 5}
!120 = !{i32 12, !121, !122, !123, !124}
!121 = !{i32 6, !"base_mip_level", i32 3, i32 0, i32 7, i32 3}
!122 = !{i32 6, !"mip_level_count", i32 3, i32 2, i32 7, i32 3}
!123 = !{i32 6, !"base_array_layer", i32 3, i32 4, i32 7, i32 5}
!124 = !{i32 6, !"array_layer_count", i32 3, i32 8, i32 7, i32 5}
!125 = !{i32 36, !126, !127, !128, !129, !130, !131, !132, !133, !134}
!126 = !{i32 6, !"Type", i32 3, i32 0, i32 7, i32 5}
!127 = !{i32 6, !"TemporalLayers", i32 3, i32 4, i32 7, i32 5}
!128 = !{i32 6, !"Flags", i32 3, i32 8, i32 7, i32 5}
!129 = !{i32 6, !"Width", i32 3, i32 12, i32 7, i32 5}
!130 = !{i32 6, !"Height", i32 3, i32 16, i32 7, i32 5}
!131 = !{i32 6, !"DepthOrArraySize", i32 3, i32 20, i32 7, i32 5}
!132 = !{i32 6, !"MipLevels", i32 3, i32 24, i32 7, i32 5}
!133 = !{i32 6, !"Format", i32 3, i32 28, i32 7, i32 5}
!134 = !{i32 6, !"SampleCount", i32 3, i32 32, i32 7, i32 5}
!135 = !{i32 1, void (%struct.texture*, i32, %struct.ResourceDesc*)* @"\01?make_default_texture_view_from_desc@@YA?AUtexture@@IUResourceDesc@@@Z", !136, void (%struct.texture*)* @rpsl_M_rpsl_triangle_Fn_main, !141}
!136 = !{!137, !138, !139, !137}
!137 = !{i32 0, !2, !2}
!138 = !{i32 1, !2, !2}
!139 = !{i32 0, !140, !2}
!140 = !{i32 7, i32 5}
!141 = !{!138, !142}
!142 = !{i32 0, !143, !2}
!143 = !{i32 6, !"backbuffer"}
!144 = !{null, !"", null, null, !145}
!145 = !{i32 0, i64 8388640}
!146 = !{void (%struct.texture*)* @rpsl_M_rpsl_triangle_Fn_main, !"rpsl_M_rpsl_triangle_Fn_main", null, null, !147}
!147 = !{i32 8, i32 15}
!148 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "desc", arg: 2, scope: !21, file: !22, line: 152, type: !51)
!149 = !DIExpression()
!150 = !DILocation(line: 152, column: 77, scope: !21)
!151 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "resourceHdl", arg: 1, scope: !21, file: !22, line: 152, type: !50)
!152 = !DILocation(line: 152, column: 51, scope: !21)
!153 = !DILocation(line: 158, column: 14, scope: !21)
!154 = !DILocation(line: 157, column: 15, scope: !21)
!155 = !DILocation(line: 157, column: 20, scope: !21)
!156 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "result", arg: 0, scope: !80, file: !22, line: 121, type: !25)
!157 = !DILocation(line: 121, column: 13, scope: !80, inlinedAt: !158)
!158 = distinct !DILocation(line: 154, column: 12, scope: !21)
!159 = !DILocation(line: 157, column: 9, scope: !21)
!160 = !DILocation(line: 157, column: 54, scope: !21)
!161 = !DILocation(line: 154, column: 12, scope: !21)
!162 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "numMips", arg: 4, scope: !80, file: !22, line: 119, type: !50)
!163 = !DILocation(line: 119, column: 103, scope: !80, inlinedAt: !158)
!164 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "arraySlices", arg: 3, scope: !80, file: !22, line: 119, type: !50)
!165 = !DILocation(line: 119, column: 85, scope: !80, inlinedAt: !158)
!166 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "resourceHdl", arg: 1, scope: !80, file: !22, line: 119, type: !50)
!167 = !DILocation(line: 119, column: 48, scope: !80, inlinedAt: !158)
!168 = !{!169}
!169 = distinct !{!169, !170, !"\01?make_default_texture_view@@YA?AUtexture@@IIII@Z: %agg.result"}
!170 = distinct !{!170, !"\01?make_default_texture_view@@YA?AUtexture@@IIII@Z"}
!171 = !DILocation(line: 123, column: 12, scope: !80, inlinedAt: !158)
!172 = !DILocation(line: 123, column: 21, scope: !80, inlinedAt: !158)
!173 = !DILocation(line: 124, column: 12, scope: !80, inlinedAt: !158)
!174 = !DILocation(line: 124, column: 23, scope: !80, inlinedAt: !158)
!175 = !DILocation(line: 125, column: 12, scope: !80, inlinedAt: !158)
!176 = !DILocation(line: 125, column: 26, scope: !80, inlinedAt: !158)
!177 = !DILocation(line: 126, column: 12, scope: !80, inlinedAt: !158)
!178 = !DILocation(line: 126, column: 18, scope: !80, inlinedAt: !158)
!179 = !DILocation(line: 127, column: 29, scope: !80, inlinedAt: !158)
!180 = !DILocation(line: 127, column: 44, scope: !80, inlinedAt: !158)
!181 = !DILocation(line: 128, column: 56, scope: !80, inlinedAt: !158)
!182 = !DILocation(line: 128, column: 29, scope: !80, inlinedAt: !158)
!183 = !DILocation(line: 128, column: 45, scope: !80, inlinedAt: !158)
!184 = !DILocation(line: 129, column: 29, scope: !80, inlinedAt: !158)
!185 = !DILocation(line: 129, column: 46, scope: !80, inlinedAt: !158)
!186 = !DILocation(line: 130, column: 29, scope: !80, inlinedAt: !158)
!187 = !DILocation(line: 130, column: 47, scope: !80, inlinedAt: !158)
!188 = !DILocation(line: 131, column: 12, scope: !80, inlinedAt: !158)
!189 = !DILocation(line: 131, column: 24, scope: !80, inlinedAt: !158)
!190 = !DILocation(line: 132, column: 12, scope: !80, inlinedAt: !158)
!191 = !DILocation(line: 132, column: 29, scope: !80, inlinedAt: !158)
!192 = !DILocation(line: 134, column: 5, scope: !80, inlinedAt: !158)
!193 = !DILocation(line: 154, column: 5, scope: !21)
!194 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "backbuffer", arg: 1, scope: !77, file: !1, line: 4, type: !25)
!195 = !DILocation(line: 4, column: 46, scope: !77)
!196 = !DILocation(line: 7, column: 5, scope: !77)
!197 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "val", arg: 2, scope: !87, file: !22, line: 292, type: !6)
!198 = !DILocation(line: 292, column: 38, scope: !87, inlinedAt: !199)
!199 = distinct !DILocation(line: 7, column: 5, scope: !77)
!200 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "t", arg: 1, scope: !87, file: !22, line: 292, type: !25)
!201 = !DILocation(line: 292, column: 28, scope: !87, inlinedAt: !199)
!202 = !DILocation(line: 294, column: 12, scope: !87, inlinedAt: !199)
!203 = !DILocation(line: 294, column: 5, scope: !87, inlinedAt: !199)
!204 = !DILocation(line: 8, column: 5, scope: !77)
!205 = !DILocation(line: 9, column: 1, scope: !77)
!206 = !DILocation(line: 4, scope: !77)
 
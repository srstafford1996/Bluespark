; SPIR-V
; Version: 1.0
; Generator: Khronos Glslang Reference Front End; 3
; Bound: 20
; Schema: 0
               OpCapability Shader
          %1 = OpExtInstImport "GLSL.std.450"
               OpMemoryModel Logical GLSL450
               OpEntryPoint Vertex %main "main" %_
               OpEntryPoint Vertex %main2 "main2" %_
               OpEntryPoint Fragment %main3 "main" %FragColor
               OpEntryPoint Fragment %main4 "main2" %FragColor
               OpSource GLSL 450
               OpMemberDecorate %gl_PerVertex 0 BuiltIn Position
               OpMemberDecorate %gl_PerVertex 1 BuiltIn PointSize
               OpMemberDecorate %gl_PerVertex 2 BuiltIn ClipDistance
               OpMemberDecorate %gl_PerVertex 3 BuiltIn CullDistance
			   OpDecorate %FragColor Location 0
               OpDecorate %gl_PerVertex Block
       %void = OpTypeVoid
          %3 = OpTypeFunction %void
      %float = OpTypeFloat 32
    %v4float = OpTypeVector %float 4
	%v4floatptr = OpTypePointer Output %v4float
       %uint = OpTypeInt 32 0
     %uint_1 = OpConstant %uint 1
%_arr_float_uint_1 = OpTypeArray %float %uint_1
%gl_PerVertex = OpTypeStruct %v4float %float %_arr_float_uint_1 %_arr_float_uint_1
%_ptr_Output_gl_PerVertex = OpTypePointer Output %gl_PerVertex
          %_ = OpVariable %_ptr_Output_gl_PerVertex Output
  %FragColor = OpVariable %v4floatptr Output
        %int = OpTypeInt 32 1
      %int_0 = OpConstant %int 0
    %float_1 = OpConstant %float 1
         %17 = OpConstantComposite %v4float %float_1 %float_1 %float_1 %float_1
    %float_2 = OpConstant %float 2
         %18 = OpConstantComposite %v4float %float_2 %float_2 %float_2 %float_2
%_ptr_Output_v4float = OpTypePointer Output %v4float
       %main = OpFunction %void None %3
          %5 = OpLabel
         %19 = OpAccessChain %_ptr_Output_v4float %_ %int_0
               OpStore %19 %17
               OpReturn
               OpFunctionEnd
      %main2 = OpFunction %void None %3
          %6 = OpLabel
         %20 = OpAccessChain %_ptr_Output_v4float %_ %int_0
               OpStore %20 %18
               OpReturn
               OpFunctionEnd
	 %main3 = OpFunction %void None %3
          %7 = OpLabel
		  	   OpStore %FragColor %17
			   OpReturn
			   OpFunctionEnd
	 %main4 = OpFunction %void None %3
          %8 = OpLabel
		  	   OpStore %FragColor %18
			   OpReturn
			   OpFunctionEnd

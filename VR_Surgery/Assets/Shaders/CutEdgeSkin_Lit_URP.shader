Shader "Custom/CutEdgeSkin_Lit_URP"
{
    Properties
    {
        _BaseColor ("Base Skin Color", Color) = (1,0.8,0.7,1)
        _CutColor  ("Cut Edge Color", Color)  = (0.45,0,0,1)
        _SmoothnessDry ("Dry Smoothness", Range(0,1)) = 0.3
        _SmoothnessWet ("Wet Smoothness", Range(0,1)) = 0.9
    }

    SubShader
    {
        Tags
        {
            "RenderPipeline"="UniversalPipeline"
            "RenderType"="Opaque"
            "Queue"="Geometry"
        }

        Pass
        {
            Name "ForwardLit"
            Tags { "LightMode"="UniversalForward" }

            HLSLPROGRAM
            #pragma vertex vert
            #pragma fragment frag

            #include "Packages/com.unity.render-pipelines.universal/ShaderLibrary/Core.hlsl"
            #include "Packages/com.unity.render-pipelines.universal/ShaderLibrary/Lighting.hlsl"

            struct Attributes
            {
                float4 positionOS : POSITION;
                float3 normalOS   : NORMAL;
                float4 color      : COLOR;
            };

            struct Varyings
            {
                float4 positionHCS : SV_POSITION;
                float3 normalWS    : TEXCOORD0;
                float3 positionWS  : TEXCOORD1;
                float  cutMask     : TEXCOORD2;
            };

            float4 _BaseColor;
            float4 _CutColor;
            float  _SmoothnessDry;
            float  _SmoothnessWet;

            Varyings vert (Attributes IN)
            {
                Varyings OUT;
                OUT.positionHCS = TransformObjectToHClip(IN.positionOS.xyz);
                OUT.positionWS  = TransformObjectToWorld(IN.positionOS.xyz);
                OUT.normalWS    = TransformObjectToWorldNormal(IN.normalOS);
                OUT.cutMask     = saturate(IN.color.r);
                return OUT;
            }
            // Varyings vert (Attributes IN)
            // {
            // Varyings OUT;
            // OUT.positionHCS = TransformObjectToHClip(IN.positionOS.xyz);
            // OUT.positionWS  = TransformObjectToWorld(IN.positionOS.xyz);
            // OUT.normalWS    = TransformObjectToWorldNormal(IN.normalOS);

            // // ✅ SAFE cut mask logic
            // // Treat uninitialized / white vertex colors as UNCUT
            // float raw = IN.color.r;

            // // If color is close to 1 (default white), force it to 0
            // raw = (raw > 0.99) ? 0.0 : raw;

            // OUT.cutMask = saturate(raw);
            // return OUT;
            // }



            half4 frag (Varyings IN) : SV_Target
            {
                // Normalize normal
                float3 normal = normalize(IN.normalWS);

                // Get main light
                Light light = GetMainLight();
                float3 lightDir = normalize(light.direction);

                // Lambert lighting
                float NdotL = saturate(dot(normal, lightDir));

                // Blend skin & cut colors
                float3 baseColor = lerp(_BaseColor.rgb, _CutColor.rgb, IN.cutMask);

                // Smoothness control (wet near cut)
                float smoothness = lerp(_SmoothnessDry, _SmoothnessWet, IN.cutMask);

                // Specular highlight
                float3 viewDir = normalize(_WorldSpaceCameraPos - IN.positionWS);
                float3 halfDir = normalize(lightDir + viewDir);
                float spec = pow(saturate(dot(normal, halfDir)), 64) * smoothness;

                float3 color =
                    baseColor * (NdotL * light.color.rgb) +
                    spec * light.color.rgb;

                return half4(color, 1);
            }
            ENDHLSL
        }
    }
}
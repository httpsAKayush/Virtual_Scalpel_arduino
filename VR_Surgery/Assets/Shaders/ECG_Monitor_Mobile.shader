Shader "Custom/ECG_Monitor_Mobile"
{
    Properties
    {
        _LineColor ("ECG Line Color", Color) = (0,1,0,1)
        _BgColor ("Background Color", Color) = (0,0,0,1)
        _Thickness ("Line Thickness", Range(0.001, 0.05)) = 0.01
        _Speed ("Scroll Speed", Float) = 1
        _Amplitude ("Amplitude", Float) = 0.25
        _BPM ("Heart Rate (BPM)", Float) = 72
    }

    SubShader
    {
        Tags { "RenderType"="Opaque" }

        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #include "UnityCG.cginc"

            fixed4 _LineColor;
            fixed4 _BgColor;
            float _Thickness;
            float _Speed;
            float _Amplitude;
            float _BPM;

            struct appdata
            {
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
            };

            struct v2f
            {
                float2 uv : TEXCOORD0;
                float4 vertex : SV_POSITION;
            };

            float ECGWave(float t)
            {
                float p = exp(-pow((t - 0.15) * 30.0, 2.0)) * 0.1;
                float q = -exp(-pow((t - 0.25) * 60.0, 2.0)) * 0.15;
                float r = exp(-pow((t - 0.3) * 120.0, 2.0)) * 0.8;
                float s = -exp(-pow((t - 0.35) * 60.0, 2.0)) * 0.2;
                float tw = exp(-pow((t - 0.6) * 20.0, 2.0)) * 0.25;

                return (p + q + r + s + tw) * _Amplitude;
            }

            v2f vert (appdata v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.uv = v.uv;
                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                float bpmPeriod = 60.0 / max(_BPM, 1.0);
                float t = _Time.y * _Speed;

                float x = frac(i.uv.x + t / bpmPeriod);
                float ecgY = 0.5 + ECGWave(x);

                float dist = abs(i.uv.y - ecgY);

                float lineMask = smoothstep(_Thickness, 0.0, dist);

                fixed4 col = lerp(_BgColor, _LineColor, lineMask);
                return col;
            }
            ENDCG
        }
    }
}
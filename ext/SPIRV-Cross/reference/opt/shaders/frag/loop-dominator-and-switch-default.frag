#version 310 es
precision mediump float;
precision highp int;

vec4 _80;

layout(location = 0) out vec4 fragColor;

void main()
{
    mediump int _18 = int(_80.x);
    vec4 _82;
    _82 = _80;
    vec4 _89;
    for (mediump int _81 = 0; _81 < _18; _82 = _89, _81++)
    {
        vec4 _83;
        switch (_18)
        {
            case 0:
            {
                vec4 _74 = _82;
                _74.y = 0.0;
                _83 = _74;
                break;
            }
            case 1:
            {
                vec4 _76 = _82;
                _76.y = 1.0;
                _83 = _76;
                break;
            }
            default:
            {
                vec4 _88;
                _88 = _82;
                for (mediump int _84 = 0; _84 < _18; )
                {
                    vec4 _72 = _88;
                    _72.y = _88.y + 0.5;
                    _88 = _72;
                    _84++;
                    continue;
                }
                _89 = _88;
                continue;
            }
        }
        vec4 _79 = _83;
        _79.y = _83.y + 0.5;
        _89 = _79;
    }
    fragColor = _82;
}


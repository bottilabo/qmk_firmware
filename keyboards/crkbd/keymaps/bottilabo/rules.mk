MOUSEKEY_ENABLE = no       # Mouse keys(+4700)
#RGBLIGHT_ENABLE = yes       # Enable WS2812 RGB underlight.

# If you want to change the display of OLED, you need to change here
SRC +=  \
        ./glcdfont-hitsuji.c \
        ./logo_reader-hitsuji.c \
        ./lib/rgb_state_reader.c \
        ./lib/layer_state_reader.c \
        #./lib/keylogger.c \
        # ./lib/mode_icon_reader.c \
        # ./lib/host_led_state_reader.c \
        # ./lib/timelogger.c \


#	.    ./glcdfont-kyomu-r.c \
#        ./logo_reader-kyomu-r.c \
#        ./glcdfont-hitsuji.c \
#        ./logo_reader-hitsuji.c \

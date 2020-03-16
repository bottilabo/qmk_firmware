/*
 * Copyright 2020 bottilabo (twitter: @bottilabo)
 * http://github.com/bottilabo/qmk-harmonize
 */
#include QMK_KEYBOARD_H

#define HAS_THUMBROW

#define IMM K_XIMMODE

//      +    +    +    *                                 *    +    +    +
//4     4    3    2    1    1      1              1      1    1    2    3    4      4
#define LAYOUT_remapper( \
ESC ,   NL5, NL4, NL3, NL2, NL1,              NR1, NR2, NR3, NR4, NR5,   NR6, NR7, BSPC,  \
TAB ,   L05, L04, L03, L02, L01,              R01, R02, R03, R04, R05,   R06, R07, BSLS,   \
CAPS,   L15, L14, L13, L12, L11,              R11, R12, R13, R14, R15,   R16, R17, ENTR,  \
LSFT,   L25, L24, L23, L22, L21,              R21, R22, R23, R24, R25,   R26,      RSFT,  \
MOUS,  LCTL,LALT,LGUI,                                            RGUI,RALT,RCTL,   ADJ, \
                       TL2,                                   TR2,                       \
       PGUP,                TL1,                         TR1,                UP,         \
HOME,  PGDN,END ,                  TL0,           TR0,                 LEFT,DOWN,  RIGT, \
INS ,DEL   ,PRNT,BRK,                                                ZENHN,KANA,MHEN,HENK  \
  ) \
  LAYOUT( \
ESC , L05, L04, L03, L02, L01,                     R01, R02, R03, R04, R05,  R06, \
CAPS, L15, L14, L13, L12, L11,                     R11, R12, R13, R14, R15,  R16, \
LGUI, L25, L24, L23, L22, L21,                     R21, R22, R23, R24, R25,  ADJ, \
                     LALT, TL1,  TL0,   TR0, TR1, LCTL  \
    )
#define LAYOUT_DEF(...)                  LAYOUT_remapper(__VA_ARGS__)

#include "harmonize.h"




#ifdef RGBLIGHT_ENABLE
//Following line allows macro to read current RGB settings
extern rgblight_config_t rgblight_config;
#endif

extern uint8_t is_master;

//#define BACK_CH 0x92
#define BACK_CH ' '

int RGB_current_mode;

void persistent_default_layer_set(uint16_t default_layer) {
  eeconfig_update_default_layer(default_layer);
  default_layer_set(default_layer);
}

// Setting ADJUST layer RGB back to default
void update_tri_layer_RGB(uint8_t layer1, uint8_t layer2, uint8_t layer3) {
  if (IS_LAYER_ON(layer1) && IS_LAYER_ON(layer2)) {
    layer_on(layer3);
  } else {
    layer_off(layer3);
  }
}

void matrix_fill_screen(struct CharacterMatrix *matrix,char ch) {
  memset(matrix->display,ch, sizeof(matrix->display));
  matrix->cursor = &matrix->display[0][0];
  matrix->dirty = true;
}

void matrix_cr(struct CharacterMatrix *matrix,char ch) {
    uint8_t cursor_col = (matrix->cursor - &matrix->display[0][0]) % MatrixCols;
    while (cursor_col++ < MatrixCols) {
      matrix_write_char_inner(matrix, ch);
    }
}


void matrix_init_user(void) {
    #ifdef RGBLIGHT_ENABLE
      RGB_current_mode = rgblight_config.mode;
    #endif
    //SSD1306 OLED init, make sure to add #define SSD1306OLED in config.h
    #ifdef SSD1306OLED
        iota_gfx_init(!has_usb());   // turns on the display
    #endif
    harmonize_init();
}

//SSD1306 OLED update loop, make sure to add #define SSD1306OLED in config.h
#ifdef SSD1306OLED

uint8_t logo_pattern = 0;
uint8_t anim_frame = 0;

// When add source files to SRC in rules.mk, you can use functions.
const char *read_layer_state(void);
const char *read_logo_ex(int pattern);
void set_keylog(uint16_t keycode, keyrecord_t *record);
const char *read_keylog(void);
const char *read_keylogs(void);
uint8_t logo_w(void); 
uint8_t logo_patterns(void);

// const char *read_mode_icon(bool swap);
// const char *read_host_led_state(void);
// void set_timelog(void);
// const char *read_timelog(void);

void matrix_scan_user(void) {
   iota_gfx_task();
   HARMONIZE_MATRIX_SCAN_USER;
}

void matrix_render_user(struct CharacterMatrix *matrix) {
  if (is_master) {
    // If you want to change the display of OLED, you need to change here
    matrix_write(matrix,"mode: ");
    matrix_write_ln(matrix, get_kb_layout());
    const char* p;
    switch(_harmonize.os_type) {
        case OS_WIN:p="Windows";break;
        case OS_MAC:p="Mac";break;
        default:p="Linux";break;
    }
    matrix_write_ln(matrix, p);

    //matrix_write_ln(matrix, read_layer_state());
    //matrix_write_ln(matrix, read_keylog());
    //matrix_write_ln(matrix, read_keylogs());
    //matrix_write_ln(matrix, read_mode_icon(keymap_config.swap_lalt_lgui));
    //matrix_write_ln(matrix, read_host_led_state());
    //matrix_write_ln(matrix, read_timelog());
  } else {
    //matrix_write(matrix, read_logo_ex(logo_pattern));
    uint8_t w = MatrixCols-logo_w();
    uint8_t padding = w-1-(anim_frame % w);
    uint8_t patid = anim_frame%logo_patterns();
    if(logo_patterns()==1)
        patid = 0;

    //if( (anim_frame%(w*2))<w )
    //    matrix_cr(matrix,BACK_CH);

    const char* p = read_logo_ex(patid);
    while(*p) {
        for(uint8_t i=0;i<padding;i++)
            matrix_write_char(matrix,BACK_CH); 

        while(*p) {
            char c = *p++;
            matrix_write_char(matrix,c); 
        }
        //matrix_write_char(matrix,'\n'); 
        matrix_cr(matrix,BACK_CH);
        p++;
    }
  }
}

void matrix_update(struct CharacterMatrix *dest, const struct CharacterMatrix *source) {
  if (memcmp(dest->display, source->display, sizeof(dest->display))) {
    memcpy(dest->display, source->display, sizeof(dest->display));
    dest->dirty = true;
  }
}

void iota_gfx_task_user(void) {
  struct CharacterMatrix matrix;
  //matrix_clear(&matrix);
  matrix_fill_screen(&matrix,BACK_CH);
  matrix_render_user(&matrix);
  matrix_update(&display, &matrix);
}
#endif//SSD1306OLED

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  HARMONIZE_PROC_RECORD_USER;
  if (record->event.pressed) {
#ifdef SSD1306OLED
    //set_keylog(keycode, record);
    logo_pattern = !logo_pattern;
    anim_frame++;
#endif
    // set_timelog();
  }


  switch (keycode) {
    case RGB_MOD:
      #ifdef RGBLIGHT_ENABLE
        if (record->event.pressed) {
          rgblight_mode(RGB_current_mode);
          rgblight_step();
          RGB_current_mode = rgblight_config.mode;
        }
      #endif
      return false;
    case RGBRST:
      #ifdef RGBLIGHT_ENABLE
        if (record->event.pressed) {
          eeconfig_update_rgblight_default();
          rgblight_enable();
          RGB_current_mode = rgblight_config.mode;
        }
      #endif
      break;
  }
  return true;
}

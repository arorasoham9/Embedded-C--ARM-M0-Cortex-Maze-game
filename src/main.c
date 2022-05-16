#include "stm32f0xx.h"
#include <string.h>
#include "lcd.h"
#include "speaker.h"
#include "midi.h"
char** generate_maze(int, int);
void destroyArray(char** arr);
//extern const Picture background; // A 240x320 background image
MIDI_Player * music;
int time_remaining;
int current_score = 0;
int high_score = 0;
int num_rows = 13;
int num_cols = 13;
int game_number = 0;
int level_score = 60;

int up = 0;
int down = 0;
int left = 0;
int right = 0;
int ball_row = 1;
int ball_col = 1;

char ** global_maze;

//===========================================================================
// GIMP images
//===========================================================================
/* GIMP RGB C-Source image dump (line.c) */
static const struct {
  unsigned int      width;
  unsigned int      height;
  unsigned int      bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
  unsigned char     pixel_data[8 * 8 * 3 + 1];
} line = {
  8, 8, 3,
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
  "\000\000\000\000\000\000\000\000\000\000\000\000",
};
/* GIMP RGB C-Source image dump (green_square.c) */

static const struct {
  unsigned int   width;
  unsigned int   height;
  unsigned int   bytes_per_pixel; // 2:RGB16, 3:RGB, 4:RGBA
  unsigned char  pixel_data[8 * 8 * 2 + 1];
} ball = {
  8, 8, 2,
  "\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_"
  "\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_"
  "\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_"
  "\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_"
  "\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_"
  "\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_"
  "\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_\262_"
  "\262_\262_\262_",
};
/* GIMP RGB C-Source image dump (white_square.c) */
static const struct {
  unsigned int   width;
  unsigned int   height;
  unsigned int   bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
  unsigned char  pixel_data[8 * 8 * 3 + 1];
} white_square = {
  8, 8, 3,
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377",
};

//===========================================================================
// I2C Functions
//===========================================================================
#define LIS3DH_ADDR 0x19  // LIS3DH ACCELEROMETER I2C ADDRESS
#define LIS3DH_CTRL_REG1 0x20
#define LIS3DH_OUT_X_L 0xA8 // MSB bit set to enable multiple register reading
#define LIS3DH_OUT_Y_L 0xAA // MSB bit set to enable multiple register reading
#define LIS3DH_OUT_Z_L 0xAC // MSB bit set to enable multiple register reading

void init_i2c(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    GPIOB->MODER |= 2<<(2*6) | 2<<(2*7);
    GPIOB->AFR[0] |= 1<<(4*6) | 1<<(4*7);

    I2C1->CR1 &= ~I2C_CR1_PE; // Disable channel
    I2C1->CR1 &= ~I2C_CR1_ANFOFF; // Turn on analog noise filter
    I2C1->CR1 &= ~I2C_CR1_ERRIE; // Disable error interrupt
    I2C1->CR1 &= ~I2C_CR1_NOSTRETCH; // Enable clock stretching

    I2C1->TIMINGR = 0;
    I2C1->TIMINGR &= ~I2C_TIMINGR_PRESC;// Clear prescaler
    I2C1->TIMINGR |= 0 << 28;           // Set prescaler to 0
    I2C1->TIMINGR |= 3 << 20;           // SCLDEL
    I2C1->TIMINGR |= 1 << 16;           // SDADEL
    I2C1->TIMINGR |= 3 << 8;            // SCLH
    I2C1->TIMINGR |= 9 << 0;            // SCLL

    // Disable both "own addresses"
    I2C1->OAR1 &= ~I2C_OAR1_OA1EN;
    I2C1->OAR1 = I2C_OAR1_OA1EN | 0x2;
    I2C1->OAR2 &= ~I2C_OAR2_OA2EN;

    I2C1->CR2 &= ~I2C_CR2_ADD10; // Configure for 7-bit mode
    I2C1->CR2 |= I2C_CR2_AUTOEND; // Enable auto end
    //I2C1->CR2 |= I2C_CR2_NACK; // Should I have this?

    I2C1->CR1 |= I2C_CR1_PE; // Enable channel
}

void i2c_waitidle(void) {
    while ( (I2C1->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY); // while busy, wait
}

void i2c_start(uint32_t devaddr, uint8_t size, uint8_t dir) {
    // dir: 0 = master requests a write transfer
    // dir: 1 = master requests a read transfer
    uint32_t tmpreg = I2C1->CR2;
    tmpreg &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES |
                I2C_CR2_RELOAD | I2C_CR2_AUTOEND |
                I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
    if (dir == 1)
        tmpreg |= I2C_CR2_RD_WRN; // Read from slave
    else
        tmpreg &= ~I2C_CR2_RD_WRN; // Write to slave
    tmpreg |= ((devaddr<<1) & I2C_CR2_SADD) | ((size << 16) & I2C_CR2_NBYTES);
    tmpreg |= I2C_CR2_START;
    I2C1->CR2 = tmpreg;
}

void i2c_stop(void) {
    if (I2C1->ISR & I2C_ISR_STOPF)
        return;
    // Master: Generate STOP bit after current byte has been transferred
    I2C1->CR2 |= I2C_CR2_STOP;
    // Wait until STOPF flag is reset
    while ( (I2C1->ISR & I2C_ISR_STOPF) == 0);
    I2C1->ICR |= I2C_ICR_STOPCF; // Write to clear STOPF flag
}

int i2c_checknack(void) {
    if ((I2C1->ISR & I2C_ISR_NACKF) == 1)
            return 1;
    else
        return 0;
}

void i2c_clearnack(void) {
    I2C1->ICR |= I2C_ICR_NACKCF;
}

int i2c_senddata(uint8_t devaddr, const void *data, uint8_t size) {
    int i;
    if (size <= 0 || data == 0) return -1;
    uint8_t *udata = (uint8_t*)data;
    i2c_waitidle();
    // Last argument is dir: 0 = sending data to the slave device
    i2c_start(devaddr, size, 0);

    for(i=0; i<size; i++){
        // TXIS bit is set by hardware when the TXDR register is empty and the
        // data to be transmitted must be written in the TXDR register. It is
        // cleared when the next data to be sent is written in the TXDR register.
        // The TXIS flag is not set when a NACK is recieved.
        int count = 0;
        while( (I2C1->ISR & I2C_ISR_TXIS) == 0){
            count +=  1;
            if (count > 1000000) return -1;
            if (i2c_checknack()) { i2c_clearnack(); i2c_stop(); return -1;}
        }
        // TXIS is cleared by writing to the TXDR register.
        I2C1->TXDR = udata[i] & I2C_TXDR_TXDATA;
    }
    // Wait until TC flag is set or the NACK flag is set
    while((I2C1->ISR & I2C_ISR_TC) == 0 && (I2C1->ISR & I2C_ISR_NACKF) == 0);
    if ((I2C1->ISR & I2C_ISR_NACKF) != 0)
        return -1;
    i2c_stop();
    return 0;
}

int i2c_recvdata(uint8_t devaddr, void *data, uint8_t size) {
    int i;
    if (size <= 0 || data == 0) return -1;
    uint8_t *udata = (uint8_t*)data;
    i2c_waitidle();
    // Last argument is dir: 1 = receiving data from the slave device.
    i2c_start(devaddr, size, 1);
    for(i=0; i<size; i++){
        int count = 0;
        while ((I2C1->ISR & I2C_ISR_RXNE) == 0){
            count += 1;
            if (count > 1000000) return -1;
            if (i2c_checknack()) { i2c_clearnack(); i2c_stop(); return -1;}
        }
        udata[i] = I2C1->RXDR;
    }
    // Wait until TX flag is set or the NACK flag is set;
    while((I2C1->ISR & I2C_ISR_TC) == 0 && (I2C1->ISR & I2C_ISR_NACKF) == 0);
    if ((I2C1->ISR & I2C_ISR_NACKF) != 0)
        return -1;
    i2c_stop();
    return 0;
}


//===========================================================================
// Accelerometer (lis3dh)
//===========================================================================
void lis3dh_setup(void){
    uint8_t ctrl_reg1_config = 0x57; // 100hz mode, enable x/y/z
    uint8_t data[2] = {LIS3DH_CTRL_REG1, ctrl_reg1_config};
    i2c_senddata(LIS3DH_ADDR, data, 2);
}

void lis3dh_get_x(int16_t* x){
    uint8_t reg[1] = {LIS3DH_OUT_X_L};
    uint8_t data[2] = {0, 0};

    i2c_senddata(LIS3DH_ADDR, reg, 1);
    i2c_recvdata(LIS3DH_ADDR, data, 2);

    *x = *((int16_t*)data);
}

void lis3dh_get_y(int16_t* y){
    uint8_t reg[1] = {LIS3DH_OUT_Y_L};
    uint8_t data[2] = {0, 0};

    i2c_senddata(LIS3DH_ADDR, reg, 1);
    i2c_recvdata(LIS3DH_ADDR, data, 2);

    *y = *((int16_t*)data);
}

void lis3dh_get_z(int16_t* z){
    uint8_t reg[1] = {LIS3DH_OUT_Z_L};
    uint8_t data[2] = {0, 0};

    i2c_senddata(LIS3DH_ADDR, reg, 1);
    i2c_recvdata(LIS3DH_ADDR, data, 2);

    *z = *((int16_t*)data);
}

//===========================================================================
// EEPROM
//===========================================================================
void eeprom_write(uint16_t loc, const char* data, uint8_t len) {
    TIM6->CR1 &= ~TIM_CR1_CEN;
    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM7->CR1 &= ~TIM_CR1_CEN;
    uint8_t new_data[34];
    new_data[0] = loc >> 8;
    new_data[1] = loc & 0xff;
    for (int i = 2; i < 34; i++) {
        new_data[i] = data[i-2];
    }
    i2c_senddata(0x50, new_data, 34);
    TIM6->CR1 |= TIM_CR1_CEN;
    TIM2->CR1 |= TIM_CR1_CEN;
    TIM7->CR1 |= TIM_CR1_CEN;
}

int eeprom_write_complete(void) {
    i2c_waitidle();
    i2c_start(0x50, 0, 0);
    // Wait until TC flag is set or the NACK flag is set
    while((I2C1->ISR & I2C_ISR_TC) == 0 && (I2C1->ISR & I2C_ISR_NACKF) == 0);
    int nack = i2c_checknack();
    if (nack) {
        i2c_clearnack();
        i2c_stop();
        return 0;
    }
    else {
        i2c_stop();
        return 1;
    }
}

void eeprom_read(uint16_t loc, char data[], uint8_t len) {
    //TIM7->CR1 &= ~TIM_CR1_CEN; // Pause keypad scanning.
    TIM6->CR1 &= ~TIM_CR1_CEN;
    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM7->CR1 &= ~TIM_CR1_CEN;

    uint8_t locat[2] = {loc >> 8, loc & 0xff};
    i2c_senddata(0x50, locat, 2);
    i2c_recvdata(0x50, data, len);

    //TIM7->CR1 |= TIM_CR1_CEN; // Resume keypad scanning.
    TIM6->CR1 |= TIM_CR1_CEN;
    TIM2->CR1 |= TIM_CR1_CEN;
    TIM7->CR1 |= TIM_CR1_CEN;
}


void eeprom_blocking_write(uint16_t loc, const char* data, uint8_t len) {
    TIM6->CR1 &= ~TIM_CR1_CEN;
    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM7->CR1 &= ~TIM_CR1_CEN;
    //TIM7->CR1 &= ~TIM_CR1_CEN; // Pause keypad scanning.
    eeprom_write(loc, data, len);
    while (!eeprom_write_complete());
    //TIM7->CR1 |= TIM_CR1_CEN; // Resume keypad scanning.
    TIM6->CR1 |= TIM_CR1_CEN;
    TIM2->CR1 |= TIM_CR1_CEN;
    TIM7->CR1 |= TIM_CR1_CEN;
}

//===========================================================================
// SPI
//===========================================================================
void init_lcd_spi(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER11 | GPIO_MODER_MODER14);
    GPIOB->MODER |= GPIO_MODER_MODER8_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER14_0;
    GPIOB->ODR |= GPIO_ODR_8 | GPIO_ODR_11 | GPIO_ODR_14;
    GPIOB->MODER |= GPIO_MODER_MODER3 | GPIO_MODER_MODER5;
    GPIOB->MODER &= ~(GPIO_MODER_MODER3_0 | GPIO_MODER_MODER5_0);

    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    SPI1->CR1 &= ~SPI_CR1_SPE;
    SPI1->CR1 &= ~SPI_CR1_BR;
    SPI1->CR1 |= SPI_CR1_MSTR;
    SPI1->CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2;
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
    SPI1->CR1 |= SPI_CR1_SPE;
}

//===========================================================================
// Push buttons
//===========================================================================
void setup_buttons(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER &= ~0x3ff;
    GPIOC->PUPDR &= ~0x3ff;
    GPIOC->PUPDR |= 0x2aa;
}

//===========================================================================
// Maze generation
//===========================================================================
void draw_maze(char** arr)
{
    //LCD_DrawPicture(0,0,&background); // if we want a non-plain background
    LCD_Clear(0xffffff);
    int x_inc = 8;
    int y_inc = 8;
    int x_pos = 0;
    int y_pos = 0;
    arr[num_rows-2][num_cols-1] = ' ';
    //arr[0][1] = ' ';
    for (int x = 0; x < num_rows; x++) {
        for (int y = 0; y < num_cols; y++) {
            /*
            if (x == (num_rows - 2) && (y == (num_cols -  1))) {
                LCD_DrawPicture(x_pos, y_pos, &line_green);
            }
            */
            if (arr[x][y] == '|') {
                LCD_DrawPicture(x_pos, y_pos, &line);

            }
            x_pos += x_inc;
        }
        y_pos += y_inc;
        x_pos = 0;
    }
}

//===========================================================================
// Game logic
//===========================================================================
void init_tim6(void) {
    TIM6->CR1 &= ~TIM_CR1_CEN;

    time_remaining = 45+game_number;
    level_score = time_remaining;
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = 48000-1;
    TIM6->ARR = 1000-1;
    TIM6->DIER |= TIM_DIER_UIE;
    NVIC->ISER[0] = 1<<TIM6_DAC_IRQn;
    TIM6->CR1 |= TIM_CR1_CEN;
}

void TIM6_DAC_IRQHandler(void) {
    char string[20];
    char score_string[20];
    TIM6->SR &= ~TIM_SR_UIF;
    time_remaining--;
    level_score -= 1;

    //sprintf(score_string, "Level: %d", level_score);
    //LCD_DrawString(140, 275, BLACK, WHITE,  (score_string), 16, 0);
    sprintf(score_string, "Score: %d", current_score);
    LCD_DrawString(140, 300, BLACK, WHITE,  (score_string), 16, 0);

    if (time_remaining == 9) {
        LCD_DrawString(5, 300, BLACK, WHITE, "Time: 9 ", 16, 0);
    }
    else if (time_remaining <= 5) {
        sprintf(string, "Time: %d", time_remaining);
        LCD_DrawString(5, 300, RED, WHITE,  (string), 16, 0);
    }
    else {
        sprintf(string, "Time: %d", time_remaining);
        LCD_DrawString(5, 300, BLACK, WHITE,  (string), 16, 0);
    }

    if (time_remaining == 0) {
        stopMusic();
        end_game();
    }
}

void next_level() {
    LCD_Clear(0xffffff);
    destroyArray(global_maze);
    TIM6->CR1 &= ~TIM_CR1_CEN;
    TIM7->CR1 &= ~TIM_CR1_CEN;
    TIM2->CR1 &= ~TIM_CR1_CEN;

    ball_row = 1;
    ball_col = 1;
    game_number += 1;

    if (num_cols < 29) {
        num_cols += 2;
    }
    else if (num_rows < 33) {
        num_rows += 2;
    }

    TIM2->CR1 |= TIM_CR1_CEN;

    start_game();
}

static void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

void end_game(void) {
    stopMusic();
    free(music);
    LCD_Clear(0xffffff);

    LCD_DrawString(80, 50, RED, WHITE,  "GAME OVER", 16, 0);
    char score_string[20];
    //current_score += level_score;
    if (current_score > high_score) {
        high_score = current_score;
        char score[5];
        sprintf(score, "%d", high_score);
        eeprom_write(0, score, 4);

        sprintf(score_string, "New high score: %d", high_score);
        LCD_DrawString(45, 150, GREEN, WHITE,  score_string, 16, 0);
    }
    else {
        sprintf(score_string, "High score: %d", high_score);
        LCD_DrawString(50, 150, BLACK, WHITE,  (score_string), 16, 0);
    }
    sprintf(score_string, "Level score: %d", level_score);
    LCD_DrawString(50, 100, BLACK, WHITE,  (score_string), 16, 0);
    sprintf(score_string, "Total score: %d", current_score);
    LCD_DrawString(50, 125, BLACK, WHITE,  (score_string), 16, 0);
    LCD_DrawString(75, 225, BLACK, WHITE,  "Play again?", 16, 0);
    LCD_DrawString(60, 250, BLACK, WHITE,  "(Press Select)", 16, 0);

    int select = GPIOC->IDR >> 4 & 1;
    while(select == 0) {
        nano_wait(1000000);
        select = GPIOC->IDR >> 4 & 1;
    }
    game_number = 0;
    ball_row = 1;
    ball_col = 1;
    num_rows = 13;
    num_cols = 13;
    current_score = 0;
    srandom(TIM2->CNT);

    game_logic();
}

void game_logic(void) {
//    startMusic(music);
    unmuteMusic(music);
    char** arr = generate_maze(num_rows, num_cols);
    draw_maze(arr);
    LCD_DrawPicture(8, 8, &ball);
    init_tim6();
    init_tim7();
    global_maze = arr;
    game_number += 1;
}

init_tim7(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    TIM7->CR1 &= ~TIM_CR1_CEN;
    TIM7->PSC = 48000-1;
    TIM7->ARR = 175-1;
    TIM7->DIER |= TIM_DIER_UIE;
    NVIC->ISER[0] = 1<<TIM7_IRQn;
    TIM7->CR1 |= TIM_CR1_CEN;
}

void TIM7_IRQHandler(void) {
    if(checkMusicStatus(music)){
//       startMusic(music);
        unmuteMusic(music);
}
    TIM7->SR &= ~TIM_SR_UIF;
    update_orientation();
    navigate_maze();
}

void update_orientation(){
    int16_t x = 0;
    int16_t y = 0;
    int16_t z = 0;

    lis3dh_get_x(&x);
    lis3dh_get_y(&y);
    lis3dh_get_z(&z);

    if (y < -15000){
        up = 1;
        down = 0;
        left = 0;
        right = 0;
    }
    else if (y > 15000){
        up = 0;
        down = 1;
        left = 0;
        right = 0;
    }
    else if (x < -15000){
        up = 0;
        down = 0;
        left = 1;
        right = 0;
    }
    else if (x > 15000){
        up = 0;
        down = 0;
        left = 0;
        right = 1;
    }
    else{
        up = 0;
        down = 0;
        left = 0;
        right = 0;
    }
    // Debug DrawString statements
    /*
    char x_string[20];
    char y_string[20];
    char z_string[20];
    sprintf(x_string, "X Accel: %d", x);
    sprintf(y_string, "Y Accel: %d", y);
    sprintf(z_string, "Z Accel: %d", z);
    LCD_DrawString(50, 150, BLACK, WHITE,  (x_string), 16, 0);
    LCD_DrawString(50, 170, BLACK, WHITE,  (y_string), 16, 0);
    LCD_DrawString(50, 190, BLACK, WHITE,  (z_string), 16, 0);
    */
}

void navigate_maze() {
    if ((ball_row == num_rows-2) && (ball_col == num_cols-1)) {
    //if ((ball_row == 1) && (ball_col == 1)) {
        //stopping music here to begin again later
        stopMusic();
        next_level();
    }
    else if (up && global_maze[ball_row-1][ball_col] == ' ') {
        LCD_DrawPicture(ball_col*8, ball_row*8, &white_square);
        ball_row -= 1;
        LCD_DrawPicture(ball_col*8, ball_row*8, &ball);
    }
    else if (down && global_maze[ball_row + 1][ball_col] == ' ') {
        LCD_DrawPicture(ball_col*8, ball_row*8, &white_square);
        ball_row += 1;
        LCD_DrawPicture(ball_col*8, ball_row*8, &ball);
    }
    else if (left && global_maze[ball_row][ball_col-1] == ' ') {
        LCD_DrawPicture(ball_col*8, ball_row*8, &white_square);
        ball_col -= 1;
        LCD_DrawPicture(ball_col*8, ball_row*8, &ball);
    }
    else if (right && global_maze[ball_row][ball_col+1] == ' ') {
        LCD_DrawPicture(ball_col*8, ball_row*8, &white_square);
        ball_col += 1;
        LCD_DrawPicture(ball_col*8, ball_row*8, &ball);
    }
}
void clear_high_score(void){
    eeprom_write(0,0,4);
}
void start_game(void)
{
    init_tim2();
//    music = startMusic();


    if (game_number == 0) {
        char score_string[20];
        eeprom_read(0, score_string, 4);
        high_score = atoi(score_string);
        LCD_Clear(0xffffff);
        LCD_DrawString(50, 200, BLACK, WHITE,  "High score: ", 16, 0);
        LCD_DrawString(150, 200, BLACK, WHITE,  &score_string, 16, 0);

        LCD_DrawString(70, 100, BLACK, WHITE,  " Start game?", 16, 0);
        LCD_DrawString(55, 125, BLACK, WHITE,  " (Press Select)", 16, 0);
        //char score_string[20];
        //sprintf(score_string, "High score: %d", high_score);

        int buttons = GPIOC->IDR >> 4 & 1;
        while(buttons == 0) {
            nano_wait(1000000);
            buttons = GPIOC->IDR >> 4 & 1;
            //checking here if player didnt start game, music ended and needs to rebegin
//            if(checkMusicStatus(music)){
//                startMusicAgain(music);
//        }
        }
        srandom(TIM2->CNT);
        game_logic();
    }
    else {
        LCD_Clear(0xffffff);
        char score_string[20];
        current_score += level_score;
        if (current_score > high_score) {
            high_score = current_score;
            char score[5];
            sprintf(score, "%d", high_score);
            eeprom_write(0, score, 4);

            sprintf(score_string, "New high score: %d", high_score);
            LCD_DrawString(45, 125, GREEN, WHITE,  score_string, 16, 0);
        }
        else {
            sprintf(score_string, "High score: %d", high_score);
            LCD_DrawString(50, 125, BLACK, WHITE,  score_string, 16, 0);
        }
        sprintf(score_string, "Level score: %d", level_score);
        LCD_DrawString(50, 75, BLACK, WHITE,  (score_string), 16, 0);
        sprintf(score_string, "Total score: %d", current_score);
        LCD_DrawString(50, 100, BLACK, WHITE,  (score_string), 16, 0);
        //sprintf(score_string, "High score:  %d", high_score);
        //LCD_DrawString(50, 125, BLACK, WHITE,  (score_string), 16, 0);
        LCD_DrawString(30, 200, BLACK, WHITE,  "Next level?", 16, 0);
        LCD_DrawString(40, 225, BLACK, WHITE,  " (left)", 16, 0);
        LCD_DrawString(165, 200, BLACK, WHITE,  "Quit?", 16, 0);
        LCD_DrawString(155, 225, BLACK, WHITE,  "(right)", 16, 0);
        int buttons = GPIOC->IDR;
        int last_pushed = buttons >> 2 & 1 ? 2 : buttons & 1 ? 1 : 0;
        while (!last_pushed) {
            nano_wait(1000000);
            buttons = GPIOC->IDR;
            last_pushed = buttons >> 2 & 1 ? 2 : buttons & 1 ? 1 : 0;
        }
        if (last_pushed == 1) {
            srandom(TIM2->CNT);
            level_score = 60;
            game_logic();
        }
        else {

            end_game();

        }
    }
}

void init_tim2() {
    TIM2->CR1 &= ~TIM_CR1_CEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 0;
    TIM2->ARR = 0xffffffff;
    TIM2->CR1 |= TIM_CR1_CEN;
}

int main(void) {
    setup_buttons();
    LCD_Setup();
    init_i2c();
//    clear_high_score();

    lis3dh_setup();
    startMusic(music);
    stopMusic();
    start_game();

}

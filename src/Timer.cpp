#include "Timer.hpp"
#include "Util.hpp"

DelayTimer::DelayTimer( timer::Id id, GpioPin &error_indicator )
    : tim( get_timer_base( id ) ), error_indicator( error_indicator )
{
    enable_clock( id );

    MODIFY_REG(
        cast16( tim->CR1 ),
        TIM_CR1_DITHEN | TIM_CR1_UIFREMAP | TIM_CR1_ARPE | TIM_CR1_OPM | TIM_CR1_URS | TIM_CR1_UDIS | TIM_CR1_CEN,
        TIM_CR1_URS | TIM_CR1_OPM
    );
    CLEAR_REG( cast16( tim->CR2 ) );

    // TODO: make this generic based on clock speed
    // for 16MHz clock, 16 clock cycles per counter cycle -> 1us/counter cycle
    cast16( tim->PSC ) = 0x10;
}

void DelayTimer::us( uint16_t us )
{
    cast16( tim->CNT ) = 0;
    cast16( tim->ARR ) = us;
    SET_BIT( cast16( tim->CR1 ), TIM_CR1_CEN );

    // wait until counter reaches auto-reload value (tim->ARR)
    while ( READ_BIT( tim->SR, TIM_SR_UIF ) == 0 )
        ;

    CLEAR_BIT( cast16( tim->SR ), TIM_SR_UIF );
}

void DelayTimer::ms( uint16_t ms )
{
    for ( int i = 0; i < ms; i++ ) {
        us( 1000 );
    }
}

TIM_TypeDef *DelayTimer::get_timer_base( timer::Id timer_id )
{
    using namespace timer;

    switch ( timer_id ) {
    case Id::Tim6:
        return TIM6;
    case Id::Tim7:
        return TIM7;
    default:
        error();
    }
}

void DelayTimer::enable_clock( timer::Id timer_id )
{
    using namespace timer;

    switch ( timer_id ) {
    case Id::Tim6:
        SET_BIT( RCC->APB1ENR1, RCC_APB1ENR1_TIM6EN );
        break;
    case Id::Tim7:
        SET_BIT( RCC->APB1ENR1, RCC_APB1ENR1_TIM7EN );
        break;
    default:
        error();
    }
}

void DelayTimer::error()
{
    while ( 1 ) {
        error_indicator.toggle();
        spin( 100'000 );
    }
}

// ------------------------------------------------------
// Trigger Timer
// ------------------------------------------------------

TriggerTimer::TriggerTimer( timer::TriggerTimerSettings settings )
    : id( settings.id ), tim( get_timer_base( settings.id ) ), error_indicator( settings.error_indicator )
{
    enable_clock( settings.id );

    MODIFY_REG(
        cast16( tim->CR1 ),
        TIM_CR1_DITHEN | TIM_CR1_UIFREMAP | TIM_CR1_ARPE | TIM_CR1_OPM | TIM_CR1_URS | TIM_CR1_UDIS | TIM_CR1_CEN,
        TIM_CR1_URS | ( settings.one_pulse_mode ? TIM_CR1_OPM : 0 )
    );

    // use update event as trigger output
    MODIFY_REG(cast16(tim->CR2), TIM_CR2_MMS, TIM_CR2_MMS_1);

    cast16( tim->CNT ) = 0;
    cast16( tim->PSC ) = settings.prescaler_value;
    cast16( tim->ARR ) = settings.auto_reload_value;
}

void TriggerTimer::start()
{
    cast16( tim->CNT ) = 0;
    SET_BIT( cast16( tim->CR1 ), TIM_CR1_CEN );
}

void TriggerTimer::stop()
{
    cast16( tim->CNT ) = 0;
    CLEAR_BIT( cast16( tim->CR1 ), TIM_CR1_CEN );
}

// TODO: code duplication
TIM_TypeDef *TriggerTimer::get_timer_base( timer::Id timer_id )
{
    using namespace timer;

    switch ( timer_id ) {
    case Id::Tim6:
        return TIM6;
    case Id::Tim7:
        return TIM7;
    default:
        error();
    }
}

// TODO: code duplication
void TriggerTimer::enable_clock( timer::Id timer_id )
{
    using namespace timer;

    switch ( timer_id ) {
    case Id::Tim6:
        SET_BIT( RCC->APB1ENR1, RCC_APB1ENR1_TIM6EN );
        break;
    case Id::Tim7:
        SET_BIT( RCC->APB1ENR1, RCC_APB1ENR1_TIM7EN );
        break;
    default:
        error();
    }
}

// TODO: code duplication
void TriggerTimer::error()
{
    while ( 1 ) {
        error_indicator.toggle();
        spin( 100'000 );
    }
}

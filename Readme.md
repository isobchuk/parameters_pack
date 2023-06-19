# parameters_pack

**Simple component to operate with template class variadic parameter pack.**

**Supported from C++17 and higher.**

## Description

Provides three public methods to operate  with template class variadic parameter pack for hardware drivers:

- value::extract - get value from the pack according to the type
- types::check - check the pack parameters according to type list
- no_duplicates - check that pack has no duplicated types

## Usage

Usually, hardware periphery has some mandatory and some extra settings.
For example, nrf5340 gpio has mandatory port, pin and direction and extra Input, Pull, Drive, Sense, McuSel settings.
To initialize extra settings only it is required, the class can be written in that way:

```cpp
// Mandatory
enum class Port { P0, P1 };
enum class Pin { Pin_0, ... Pin_31 };
enum class Mode { Input, Output };

// Extra
enum class Input { Connect, Disconnect };
enum class Pull { No, Down, Up = 3 };
enum class Drive { SOS1, HOS1, SOH1, HOH1, DOS1, DOH1, SOD1, HOD1, EDE1 = 11 };
enum class Sense { Disabled, High = 2, Low };
enum class McuSel { AppMCU, NetworkMCU, Peripheral, TND };

// Class declaration
template <const Peripherals<Port> port, const Pin pin, const Mode mode, const auto... params>
class Gpio {
...
}
```

In this case, we can add extra parameters by requirement in any order:

```cpp
// Without extra
gpio::Gpio<driver::Peripherals(gpio::Port::P0, mcu::driver::Security::SECURE), gpio::Pin::Pin_31, gpio::Mode::Output> led{};
// Added two extra
gpio::Gpio<driver::Peripherals(gpio::Port::P0, mcu::driver::Security::SECURE), gpio::Pin::Pin_5, gpio::Mode::Input, gpio::Pull::Up, gpio::McuSel::Peripheral> uartRx{};
```

However, it is needed to operate with this pack inside the class.

First of all, the parameters should be checked for:

- only parameters with correct types were passed
- no repetitions in the pack

With the component, it can be done in class declaration (with types::check and no_duplicates):

```cpp
template <const Peripherals<Port> port, const Pin pin, const Mode mode, const auto... params>
requires(iso::parameters_pack::types<Input, Pull, Drive, Sense, McuSel>::check(params...) && iso::parameters_pack::no_duplicates(params...))
class Gpio { 
    ...
}
```

Secondly, the values have to be extracted from the pack (value::extract):

```cpp
template <const Peripherals<Port> port, const Pin pin, const Mode mode, const auto... params>
requires(iso::parameters_pack::types<Input, Pull, Drive, Sense, McuSel>::check(params...) && iso::parameters_pack::no_duplicates(params...))
class Gpio { 
  static constexpr auto INPUT = iso::parameters_pack::value<Input, Input::Disconnect>::extract(params...);
  static constexpr auto PULL = iso::parameters_pack::value<Pull>::extract(params...);
  static constexpr auto DRIVE = iso::parameters_pack::value<Drive>::extract(params...);
  static constexpr auto SENSE = iso::parameters_pack::value<Sense>::extract(params...);
  static constexpr auto MCUSEL = iso::parameters_pack::value<McuSel>::extract(params...);
}
```

If the parameter pack has value of the required type, the value will be returned. In other case, the default value of the type will be returned.
Apart from that, if the default type value is different from the hardware representation, the another default value can be passed (see INPUT).

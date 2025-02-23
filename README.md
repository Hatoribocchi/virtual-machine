Example:

```
int main( )
{
    VM::CProgramBuilder Builder;

    const std::vector<uint8_t>& vecBytecode = Builder
        .Instruction( VM::EInstructions::PUSH )
        .Bit32( 10 )

        .Instruction( VM::EInstructions::PUSH )
        .Bit32( 20 )

        .Instruction( VM::EInstructions::ADD )

        .Instruction( VM::EInstructions::POPREG )
        .Bit8( VM::ERegisters::R0 )

        .Instruction( VM::EInstructions::PUSH )
        .Bit32( 5 )

        .Instruction( VM::EInstructions::PUSHREG )
        .Bit8( VM::ERegisters::R0 )

        .Instruction( VM::EInstructions::ADD )

        .Instruction( VM::EInstructions::POPREG )
        .Bit8( VM::ERegisters::R0 )

        .Instruction( VM::EInstructions::PUSH )
        .Bit32( 35 )

        .Instruction( VM::EInstructions::PUSHREG )
        .Bit8( VM::ERegisters::R0 )

        .Instruction( VM::EInstructions::CMP )

        .Instruction( VM::EInstructions::JNZ )
        .Bit32( 0 )

        .Instruction( VM::EInstructions::HALT )
        .GetBytecode( );

    VM::CMachine Machine( vecBytecode );

    try
    {
        Machine.Execute( );
    }
    catch ( const std::exception& e )
    {
        std::cerr << "VM error: " << e.what( ) << std::endl;
    }

    return 0;
}
```

#include <iostream>
#include <vector>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>
#include <functional>
#include <variant>
#include <string>
#include <map>

namespace VM
{
    enum class EInstructions
    {
        HALT,
        PUSH,
        PUSHREG,
        MOV,
        LEA,
        POP,
        POPREG,
        ADD,
        SUB,
        MUL,
        DIV,
        AND,
        OR,
        NOT,
        SHL,
        SHR,
        JMP,
        JZ,
        JNZ,
        CMP
    };

    enum ERegisters : uint8_t
    {
        R0,
        R1,
        R2,
        R3,
        R4,
        R5,
        R6,
        R7
    };

    class CProgramBuilder
    {
    public:
        CProgramBuilder& Instruction( EInstructions Instruction )
        {
            vecBytecode.push_back( static_cast< uint8_t >( Instruction ) );
            return *this;
        }

        CProgramBuilder& Bit8( uint8_t Value )
        {
            vecBytecode.push_back( Value );
            return *this;
        }

        CProgramBuilder& Bit32( int32_t Value )
        {
            for ( int i = 0; i < 4; ++i )
            {
                vecBytecode.push_back( static_cast< uint8_t >( Value & 0xFF ) );
                Value >>= 8;
            }

            return *this;
        }

        const std::vector<uint8_t>& GetBytecode( ) const
        {
            return vecBytecode;
        }

    private:
        std::vector<uint8_t> vecBytecode{};
    };

    template <typename T, size_t iSize> struct FixedStack_t
    {
        __forceinline void Push( T const& Element )
        {
            if ( iIndex >= iSize )
                throw std::out_of_range( "Stack overflow" );

            arrElements.at( iIndex++ ) = Element;
        }

        __forceinline T Pop( )
        {
            if ( iIndex == 0 )
                throw std::out_of_range( "Stack underflow" );

            return arrElements.at( --iIndex );
        }

        __forceinline bool IsEmpty( ) const
        {
            return iIndex == 0;
        }

        __forceinline size_t Size( ) const
        {
            return iIndex;
        }

        __forceinline T& Top( )
        {
            if ( iIndex == 0 )
                throw std::out_of_range( "Stack underflow" );

            return arrElements.at( iIndex - 1 );
        }

        std::array<T, iSize>    arrElements{ };
        size_t                  iIndex{ };
    };

    class CMachine
    {
    public:
        CMachine( const std::vector<uint8_t>& vecProgram ) : vecCode( vecProgram ) { }

        void Execute( )
        {
            while ( iInstructionPointer < vecCode.size( ) )
            {
                EInstructions Instruction = static_cast< EInstructions >( vecCode.at( iInstructionPointer++ ) );

                switch ( Instruction )
                {
                case EInstructions::HALT:
                    return;
                case EInstructions::PUSH:
                {
                    int32_t iValue = ReadBit32( );
                    Stack.Push( iValue );
                    break;
                }
                case EInstructions::PUSHREG:
                {
                    uint8_t Reg = ReadBit8( );
                    if ( Reg >= arrRegisters.size( ) )
                        throw std::runtime_error( "Invalid register" );

                    Stack.Push( arrRegisters.at( Reg ) );
                    break;
                }
                case EInstructions::MOV:
                {
                    uint8_t uiDstReg = ReadBit8( );
                    uint8_t uiSrcReg = ReadBit8( );

                    if ( uiDstReg >= arrRegisters.size( ) || uiSrcReg >= arrRegisters.size( ) )
                        throw std::runtime_error( "Invalid register" );

                    arrRegisters.at( uiDstReg ) = arrRegisters.at( uiSrcReg );
                    break;
                }
                case EInstructions::LEA:
                {
                    uint8_t uiReg = ReadBit8( );
                    int32_t iAddress = ReadBit32( );

                    if ( uiReg >= arrRegisters.size( ) )
                        throw std::runtime_error( "Invalid register" );

                    arrRegisters.at( uiReg ) = iAddress;
                    break;
                }
                case EInstructions::POP:
                {
                    Stack.Pop( );
                    break;
                }
                case EInstructions::POPREG:
                {
                    uint8_t uiReg = ReadBit8( );
                    if ( uiReg >= arrRegisters.size( ) )
                        throw std::runtime_error( "Invalid register" );

                    arrRegisters.at( uiReg ) = Stack.Pop( );
                    break;
                }
                case EInstructions::ADD:
                {
                    if ( Stack.Size( ) < 2 )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iA = Stack.Pop( );
                    int32_t iB = Stack.Pop( );

                    Stack.Push( iA + iB );
                    break;
                }
                case EInstructions::SUB:
                {
                    if ( Stack.Size( ) < 2 )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iA = Stack.Pop( );
                    int32_t iB = Stack.Pop( );

                    Stack.Push( iB - iA );
                    break;
                }
                case EInstructions::MUL:
                {
                    if ( Stack.Size( ) < 2 )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iA = Stack.Pop( );
                    int32_t iB = Stack.Pop( );

                    Stack.Push( iA * iB );
                    break;
                }
                case EInstructions::DIV:
                {
                    if ( Stack.Size( ) < 2 )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iA = Stack.Pop( );
                    int32_t iB = Stack.Pop( );

                    if ( iA == 0 )
                        throw std::runtime_error( "Division by zero" );

                    Stack.Push( iB / iA );
                    break;
                }
                case EInstructions::AND:
                {
                    if ( Stack.Size( ) < 2 )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iA = Stack.Pop( );
                    int32_t iB = Stack.Pop( );

                    Stack.Push( iA & iB );
                    break;
                }
                case EInstructions::OR:
                {
                    if ( Stack.Size( ) < 2 )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iA = Stack.Pop( );
                    int32_t iB = Stack.Pop( );

                    Stack.Push( iA | iB );
                    break;
                }
                case EInstructions::NOT:
                {
                    if ( Stack.IsEmpty( ) )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iA = Stack.Pop( );
                    Stack.Push( ~iA );
                    break;
                }
                case EInstructions::SHL:
                {
                    if ( Stack.Size( ) < 2 )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iA = Stack.Pop( );
                    int32_t iB = Stack.Pop( );

                    Stack.Push( iB << iA );
                    break;
                }
                case EInstructions::SHR:
                {
                    if ( Stack.Size( ) < 2 )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iA = Stack.Pop( );
                    int32_t iB = Stack.Pop( );

                    Stack.Push( iB >> iA );
                    break;
                }
                case EInstructions::JMP:
                {
                    if ( Stack.IsEmpty( ) )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iOffset = ReadBit32( );
                    iInstructionPointer = iOffset;
                    break;
                }
                case EInstructions::JZ:
                {
                    if ( Stack.IsEmpty( ) )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iOffset = ReadBit32( );
                    if ( Stack.Top( ) == 0 )
                        iInstructionPointer = iOffset;

                    break;
                }
                case EInstructions::JNZ:
                {
                    if ( Stack.IsEmpty( ) )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iOffset = ReadBit32( );
                    if ( Stack.Top( ) != 0 )
                        iInstructionPointer = iOffset;

                    break;
                }
                case EInstructions::CMP:
                {
                    if ( Stack.Size( ) < 2 )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iA = Stack.Pop( );
                    int32_t iB = Stack.Pop( );

                    Stack.Push( static_cast< int32_t >( iB == iA ) );
                    break;
                }
                default:
                    throw std::runtime_error( "Unknown instruction" );
                }

#ifdef _DEBUG
                Trace( Instruction );
#endif
            }
        }

        const auto GetStack( )
        {
            return Stack.arrElements;
        }

        const auto GetRegister( ERegisters Register )
        {
            return arrRegisters.at( Register );
        }

    private:
        FixedStack_t<int32_t, 2048> Stack{ };
        std::vector<uint8_t>        vecCode{ };
        std::array<int32_t, 8>      arrRegisters{ };
        size_t                      iInstructionPointer{ };

        uint8_t ReadBit8( )
        {
            return vecCode.at( iInstructionPointer++ );
        }

        int32_t ReadBit32( )
        {
            int32_t iValue = 0;

            for ( int i = 0; i < 4; ++i )
                iValue |= static_cast< int32_t >( vecCode.at( iInstructionPointer++ ) ) << ( 8 * i );

            return iValue;
        }

        void Trace( EInstructions Instruction )
        {
            std::cout
                << "Instruction: " << static_cast< int >( Instruction )
                << ", IP: " << iInstructionPointer
                << ", Stack size: " << Stack.Size( )
                << std::endl;
        }
    };
}

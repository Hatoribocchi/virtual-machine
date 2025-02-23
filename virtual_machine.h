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
            if ( m_iIndex >= iSize )
                throw std::out_of_range( "Stack overflow" );

            m_arrElements.at( m_iIndex++ ) = Element;
        }

        __forceinline T Pop( )
        {
            if ( m_iIndex == 0 )
                throw std::out_of_range( "Stack underflow" );

            return m_arrElements.at( --m_iIndex );
        }

        __forceinline bool IsEmpty( ) const
        {
            return m_iIndex == 0;
        }

        __forceinline size_t Size( ) const
        {
            return m_iIndex;
        }

        __forceinline T& Top( )
        {
            if ( m_iIndex == 0 )
                throw std::out_of_range( "Stack underflow" );

            return m_arrElements.at( m_iIndex - 1 );
        }

        std::array<T, iSize>    m_arrElements{ };
        size_t                  m_iIndex{ };
    };

    class CMachine
    {
    public:
        CMachine( const std::vector<uint8_t>& vecProgram ) : m_vecCode( vecProgram ) { }

        void Execute( )
        {
            while ( m_iInstructionPointer < m_vecCode.size( ) )
            {
                EInstructions Instruction = static_cast< EInstructions >( m_vecCode.at( m_iInstructionPointer++ ) );

                switch ( Instruction )
                {
                case EInstructions::HALT:
                    return;
                case EInstructions::PUSH:
                {
                    int32_t iValue = ReadBit32( );
                    m_Stack.Push( iValue );
                    break;
                }
                case EInstructions::PUSHREG:
                {
                    uint8_t Reg = ReadBit8( );
                    if ( Reg >= m_arrRegisters.size( ) )
                        throw std::runtime_error( "Invalid register" );

                    m_Stack.Push( m_arrRegisters.at( Reg ) );
                    break;
                }
                case EInstructions::MOV:
                {
                    uint8_t uiDstReg = ReadBit8( );
                    uint8_t uiSrcReg = ReadBit8( );

                    if ( uiDstReg >= m_arrRegisters.size( ) || uiSrcReg >= m_arrRegisters.size( ) )
                        throw std::runtime_error( "Invalid register" );

                    m_arrRegisters.at( uiDstReg ) = m_arrRegisters.at( uiSrcReg );
                    break;
                }
                case EInstructions::LEA:
                {
                    uint8_t uiReg = ReadBit8( );
                    int32_t iAddress = ReadBit32( );

                    if ( uiReg >= m_arrRegisters.size( ) )
                        throw std::runtime_error( "Invalid register" );

                    m_arrRegisters.at( uiReg ) = iAddress;
                    break;
                }
                case EInstructions::POP:
                {
                    m_Stack.Pop( );
                    break;
                }
                case EInstructions::POPREG:
                {
                    uint8_t uiReg = ReadBit8( );
                    if ( uiReg >= m_arrRegisters.size( ) )
                        throw std::runtime_error( "Invalid register" );

                    m_arrRegisters.at( uiReg ) = m_Stack.Pop( );
                    break;
                }
                case EInstructions::ADD:
                {
                    if ( m_Stack.Size( ) < 2 )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iA = m_Stack.Pop( );
                    int32_t iB = m_Stack.Pop( );

                    m_Stack.Push( iA + iB );
                    break;
                }
                case EInstructions::SUB:
                {
                    if ( m_Stack.Size( ) < 2 )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iA = m_Stack.Pop( );
                    int32_t iB = m_Stack.Pop( );

                    m_Stack.Push( iB - iA );
                    break;
                }
                case EInstructions::MUL:
                {
                    if ( m_Stack.Size( ) < 2 )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iA = m_Stack.Pop( );
                    int32_t iB = m_Stack.Pop( );

                    m_Stack.Push( iA * iB );
                    break;
                }
                case EInstructions::DIV:
                {
                    if ( m_Stack.Size( ) < 2 )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iA = m_Stack.Pop( );
                    int32_t iB = m_Stack.Pop( );

                    if ( iA == 0 )
                        throw std::runtime_error( "Division by zero" );

                    m_Stack.Push( iB / iA );
                    break;
                }
                case EInstructions::AND:
                {
                    if ( m_Stack.Size( ) < 2 )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iA = m_Stack.Pop( );
                    int32_t iB = m_Stack.Pop( );

                    m_Stack.Push( iA & iB );
                    break;
                }
                case EInstructions::OR:
                {
                    if ( m_Stack.Size( ) < 2 )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iA = m_Stack.Pop( );
                    int32_t iB = m_Stack.Pop( );

                    m_Stack.Push( iA | iB );
                    break;
                }
                case EInstructions::NOT:
                {
                    if ( m_Stack.IsEmpty( ) )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iA = m_Stack.Pop( );
                    m_Stack.Push( ~iA );
                    break;
                }
                case EInstructions::SHL:
                {
                    if ( m_Stack.Size( ) < 2 )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iA = m_Stack.Pop( );
                    int32_t iB = m_Stack.Pop( );

                    m_Stack.Push( iB << iA );
                    break;
                }
                case EInstructions::SHR:
                {
                    if ( m_Stack.Size( ) < 2 )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iA = m_Stack.Pop( );
                    int32_t iB = m_Stack.Pop( );

                    m_Stack.Push( iB >> iA );
                    break;
                }
                case EInstructions::JMP:
                {
                    if ( m_Stack.IsEmpty( ) )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iOffset = ReadBit32( );
                    m_iInstructionPointer = iOffset;
                    break;
                }
                case EInstructions::JZ:
                {
                    if ( m_Stack.IsEmpty( ) )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iOffset = ReadBit32( );
                    if ( m_Stack.Top( ) == 0 )
                        m_iInstructionPointer = iOffset;

                    break;
                }
                case EInstructions::JNZ:
                {
                    if ( m_Stack.IsEmpty( ) )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iOffset = ReadBit32( );
                    if ( m_Stack.Top( ) != 0 )
                        m_iInstructionPointer = iOffset;

                    break;
                }
                case EInstructions::CMP:
                {
                    if ( m_Stack.Size( ) < 2 )
                        throw std::runtime_error( "Stack underflow" );

                    int32_t iA = m_Stack.Pop( );
                    int32_t iB = m_Stack.Pop( );

                    m_Stack.Push( static_cast< int32_t >( iB == iA ) );
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
            return m_Stack.m_arrElements;
        }

        const auto GetRegister( ERegisters Register )
        {
            return m_arrRegisters.at( Register );
        }

    private:
        FixedStack_t<int32_t, 2048> m_Stack{ };
        std::vector<uint8_t>        m_vecCode{ };
        std::array<int32_t, 8>      m_arrRegisters{ };
        size_t                      m_iInstructionPointer{ };

        uint8_t ReadBit8( )
        {
            return m_vecCode.at( m_iInstructionPointer++ );
        }

        int32_t ReadBit32( )
        {
            int32_t iValue = 0;

            for ( int i = 0; i < 4; ++i )
                iValue |= static_cast< int32_t >( m_vecCode.at( m_iInstructionPointer++ ) ) << ( 8 * i );

            return iValue;
        }

        void Trace( EInstructions Instruction )
        {
            std::cout
                << "Instruction: "  << static_cast< int >( Instruction )
                << ", IP: "         << m_iInstructionPointer
                << ", Stack size: " << m_Stack.Size( )
                << std::endl;
        }
    };
}

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

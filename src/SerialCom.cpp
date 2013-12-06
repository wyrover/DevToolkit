#include "StdAfx.h"
#include "SerialCom.h"
#include "Communication.h"
CSerialCom::CSerialCom( void )
{

}

CSerialCom::~CSerialCom( void )
{
}

CSerialFactory::CSerialFactory()
{

}

CSerialFactory::~CSerialFactory()
{

}

ConcreteSerialFactory::ConcreteSerialFactory()
{

}

ConcreteSerialFactory::~ConcreteSerialFactory()
{

}

CSerialCom* ConcreteSerialFactory::CreateSerial()
{
    return new CSerial;
}

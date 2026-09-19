#pragma once
#include <exception>
#include <string>

class PizzeriaException : public std::exception
{
public:
    PizzeriaException(std::string message);
    const char *what() const noexcept override;

private:
    std::string _message;
};

class OrderException : public PizzeriaException
{
public:
    explicit OrderException(std::string message) : PizzeriaException(message) {}
};

class EmptyOrderException : public OrderException
{
public:
    EmptyOrderException() : OrderException("Cannot process an empty order") {}
};

class NoSuchOrderException : public OrderException
{
public:
    NoSuchOrderException() : OrderException("No such order found") {}
};

class PaymentException : public PizzeriaException
{
public:
    explicit PaymentException(std::string message) : PizzeriaException(message) {}
};

class PaymentDeclinedException : public PaymentException
{
public:
    PaymentDeclinedException() : PaymentException("Payment was declined") {}
};

class InsufficientFundsException : public PaymentException
{
public:
    InsufficientFundsException() : PaymentException("Insufficient funds") {}
};

class DeliveryException : public PizzeriaException
{
public:
    explicit DeliveryException(std::string message) : PizzeriaException(message) {}
};

class NoCouriersAvailableException : public DeliveryException
{
public:
    NoCouriersAvailableException() : DeliveryException("No couriers are available") {}
};

class DeliveryZoneNotFoundException : public DeliveryException
{
public:
    DeliveryZoneNotFoundException() : DeliveryException("Delivery zone not found") {}
};

class InventoryException : public PizzeriaException
{
public:
    explicit InventoryException(std::string message) : PizzeriaException(message) {}
};

class ItemOutOfStockException : public InventoryException
{
public:
    ItemOutOfStockException() : InventoryException("Requested item is out of stock") {}
};

class StaffException : public PizzeriaException
{
public:
    explicit StaffException(std::string message) : PizzeriaException(message) {}
};
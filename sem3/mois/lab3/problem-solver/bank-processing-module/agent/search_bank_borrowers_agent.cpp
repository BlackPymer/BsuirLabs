#include "search_bank_borrowers_agent.hpp"

#include <sc-memory/sc_memory.hpp>
#include <keynodes/bank_keynodes.hpp>

ScAddr SearchBankBorrowersAgent::GetActionClass() const
{
    return BankKeynodes::action_search_bank_borrowers;
}

ScResult SearchBankBorrowersAgent::DoProgram(ScAction &action)
{
    auto const &[setAddr] = action.GetArguments<1>();

    if (!m_context.IsElement(setAddr))
    {
        m_logger.Error("Bank not specified");
        return action.FinishWithError();
    }

    m_logger.Debug("Starting searching for bank borrowers");

    ScAddrVector elements;
    ScIterator3Ptr const it3 = m_context.CreateIterator3(
        setAddr,
        ScType::ConstPermPosArc,
        ScType::Unknown);
    while (it3->Next())
    {
        ScAddr const &elementAddr = it3->Get(2);
        elements.push_back(elementAddr);
    }
}
#include "search_bank_borrowers_agent.hpp"

#include <sc-memory/sc_memory.hpp>

#include "keynodes/bank_keynodes.hpp"

ScAddr SearchBankBorrowersAgent::GetActionClass() const
{
    return BankKeynodes::action_search_bank_borrowers;
}

ScResult SearchBankBorrowersAgent::DoProgram(ScAction &action)
{
    auto const &[bankAddr] = action.GetArguments<1>();

    if (!m_context.IsElement(bankAddr))
    {
        m_logger.Error("Bank is not specified");
        return action.FinishWithError();
    }
    m_logger.Debug("Searching borrowers of the bank");

    ScStructure result = m_context.GenerateStructure();
    result << BankKeynodes::concept_bank_client;

    ScIterator5Ptr const creditsIt = m_context.CreateIterator5(
        ScType::Unknown,
        ScType::ConstCommonArc,
        bankAddr,
        ScType::ConstPermPosArc,
        BankKeynodes::nrel_has_owner);

    size_t foundCount = 0;
    while (creditsIt->Next())
    {
        ScAddr const &creditAddr = creditsIt->Get(0);

        ScIterator5Ptr const borrowersIt = m_context.CreateIterator5(
            creditAddr,
            ScType::ConstCommonArc,
            ScType::Unknown,
            ScType::ConstPermPosArc,
            BankKeynodes::nrel_has_borrower);

        while (borrowersIt->Next())
        {
            ScAddr const &borrowerAddr = borrowersIt->Get(2);

            if (result.Has(borrowerAddr, ScType::ConstPermPosArc))
                continue;

            result << borrowerAddr;
            ++foundCount;
        }
    }

    m_logger.Debug("Found", foundCount, "borrowers");

    action.SetResult(result);
    return action.FinishSuccessfully();
}

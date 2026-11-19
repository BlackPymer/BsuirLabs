#include "search_accounts_with_currency_agent.hpp"

#include <sc-memory/sc_memory.hpp>

#include "keynodes/bank_keynodes.hpp"

ScAddr SearchAccountsWithCurrencyAgent::GetActionClass() const
{
    return BankKeynodes::action_search_accounts_with_currency;
}

ScResult SearchAccountsWithCurrencyAgent::DoProgram(ScAction &action)
{
    auto const &[bankAddr, currencyAddr] = action.GetArguments<2>();

    if (!m_context.IsElement(bankAddr))
    {
        m_logger.Error("Bank is not specified");
        return action.FinishWithError();
    }

    if (!m_context.IsElement(currencyAddr))
    {
        m_logger.Error("Currency is not specified");
        return action.FinishWithError();
    }

    m_logger.Debug("Searching accounts of the bank in the specified currency");

    ScStructure result = m_context.GenerateStructure();
    result << BankKeynodes::concept_bank_account;

    ScIterator5Ptr const accountsIt = m_context.CreateIterator5(
        ScType::Unknown,
        ScType::ConstCommonArc,
        bankAddr,
        ScType::ConstPermPosArc,
        BankKeynodes::nrel_belongs_to_bank);

    size_t foundCount = 0;
    while (accountsIt->Next())
    {
        ScAddr const &accountAddr = accountsIt->Get(0);

        ScIterator5Ptr const currencyIt = m_context.CreateIterator5(
            accountAddr,
            ScType::ConstCommonArc,
            currencyAddr,
            ScType::ConstPermPosArc,
            BankKeynodes::nrel_has_currency);

        if (!currencyIt->Next())
            continue;

        if (!result.Has(accountAddr, ScType::ConstPermPosArc))
        {
            result << accountAddr;
            ++foundCount;
        }
    }

    m_logger.Debug("Found", foundCount, "accounts");

    action.SetResult(result);
    return action.FinishSuccessfully();
}

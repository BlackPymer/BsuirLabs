#include <sc-memory/test/sc_test.hpp>

#include <sc-memory/sc_memory.hpp>

#include "agent/search_accounts_with_currency_agent.hpp"
#include "keynodes/bank_keynodes.hpp"

class AccountsAgentTest : public ScMemoryTest
{
protected:
    void SetUp() override
    {
        ScMemoryTest::SetUp();
        m_ctx->SubscribeAgent<SearchAccountsWithCurrencyAgent>();
    }

    void TearDown() override
    {
        m_ctx->UnsubscribeAgent<SearchAccountsWithCurrencyAgent>();
        ScMemoryTest::TearDown();
    }
};

namespace
{
    void GenerateRelation(
        ScMemoryContext &context,
        ScAddr const &from,
        ScAddr const &to,
        ScAddr const &relation)
    {
        ScAddr const arcAddr = context.GenerateConnector(ScType::ConstCommonArc, from, to);
        context.GenerateConnector(ScType::ConstPermPosArc, relation, arcAddr);
    }

} // namespace

TEST_F(AccountsAgentTest, FinishedSuccessfully)
{
    ScAddr const bankAddr = m_ctx->GenerateNode(ScType::ConstNode);
    ScAddr const currencyAddr = m_ctx->GenerateNode(ScType::ConstNode);

    ScAddr const matchingAccountAddr = m_ctx->GenerateNode(ScType::ConstNode);
    GenerateRelation(*m_ctx, matchingAccountAddr, bankAddr, BankKeynodes::nrel_belongs_to_bank);
    GenerateRelation(*m_ctx, matchingAccountAddr, currencyAddr, BankKeynodes::nrel_has_currency);

    ScAddr const otherCurrencyAddr = m_ctx->GenerateNode(ScType::ConstNode);
    ScAddr const wrongCurrencyAccountAddr = m_ctx->GenerateNode(ScType::ConstNode);
    GenerateRelation(*m_ctx, wrongCurrencyAccountAddr, bankAddr, BankKeynodes::nrel_belongs_to_bank);
    GenerateRelation(*m_ctx, wrongCurrencyAccountAddr, otherCurrencyAddr, BankKeynodes::nrel_has_currency);

    ScAddr const otherBankAddr = m_ctx->GenerateNode(ScType::ConstNode);
    ScAddr const otherBankAccountAddr = m_ctx->GenerateNode(ScType::ConstNode);
    GenerateRelation(*m_ctx, otherBankAccountAddr, otherBankAddr, BankKeynodes::nrel_belongs_to_bank);
    GenerateRelation(*m_ctx, otherBankAccountAddr, currencyAddr, BankKeynodes::nrel_has_currency);

    ScAction action = m_ctx->GenerateAction(BankKeynodes::action_search_accounts_with_currency);
    action.SetArguments(bankAddr, currencyAddr);

    EXPECT_TRUE(action.InitiateAndWait());
    EXPECT_TRUE(action.IsFinishedSuccessfully());

    ScStructure const result = action.GetResult();
    EXPECT_FALSE(result.IsEmpty());
    EXPECT_TRUE(result.Has(BankKeynodes::concept_bank_account));
    EXPECT_TRUE(result.Has(matchingAccountAddr));
    EXPECT_FALSE(result.Has(wrongCurrencyAccountAddr));
    EXPECT_FALSE(result.Has(otherBankAccountAddr));
}

TEST_F(AccountsAgentTest, FailsWithoutArguments)
{
    ScAction action = m_ctx->GenerateAction(BankKeynodes::action_search_accounts_with_currency);
    action.InitiateAndWait();

    EXPECT_TRUE(action.IsFinishedWithError());
}

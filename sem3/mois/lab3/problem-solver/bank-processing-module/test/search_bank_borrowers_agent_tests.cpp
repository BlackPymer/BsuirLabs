// Тесты агента поиска заёмщиков банка.
//
// Как и в тестах агента счетов, используется пустая sc-память:
// ScMemoryTest сам создаёт все нужные keynode-элементы.

#include <sc-memory/test/sc_test.hpp>

#include <sc-memory/sc_memory.hpp>

#include "agent/search_bank_borrowers_agent.hpp"
#include "keynodes/bank_keynodes.hpp"

class BorrowersAgentTest : public ScMemoryTest
{
protected:
    void SetUp() override
    {
        ScMemoryTest::SetUp();
        m_ctx->SubscribeAgent<SearchBankBorrowersAgent>();
    }

    void TearDown() override
    {
        m_ctx->UnsubscribeAgent<SearchBankBorrowersAgent>();
        ScMemoryTest::TearDown();
    }
};

namespace
{

// from --common arc--> to
// relation --perm pos arc--> (эта дуга)
void GenerateRelation(
    ScMemoryContext & context,
    ScAddr const & from,
    ScAddr const & to,
    ScAddr const & relation)
{
    ScAddr const arcAddr = context.GenerateConnector(ScType::ConstCommonArc, from, to);
    context.GenerateConnector(ScType::ConstPermPosArc, relation, arcAddr);
}

// Сколько раз элемент входит в структуру (проверка отсутствия дублей).
size_t CountMembership(
    ScMemoryContext & context,
    ScAddr const & structureAddr,
    ScAddr const & elementAddr)
{
    size_t count = 0;
    ScIterator3Ptr const it = context.CreateIterator3(
        structureAddr,
        ScType::ConstPermPosArc,
        elementAddr);
    while (it->Next())
        ++count;
    return count;
}

}  // namespace

TEST_F(BorrowersAgentTest, FinishedSuccessfully)
{
    // Банк, заёмщиков которого ищем.
    ScAddr const bankAddr = m_ctx->GenerateNode(ScType::ConstNode);

    // Первый заёмщик: у него два кредита в этом банке.
    ScAddr const firstBorrowerAddr = m_ctx->GenerateNode(ScType::ConstNode);
    ScAddr const firstCreditAddr = m_ctx->GenerateNode(ScType::ConstNode);
    GenerateRelation(*m_ctx, firstCreditAddr, bankAddr, BankKeynodes::nrel_has_owner);
    GenerateRelation(*m_ctx, firstCreditAddr, firstBorrowerAddr, BankKeynodes::nrel_has_borrower);

    ScAddr const secondCreditAddr = m_ctx->GenerateNode(ScType::ConstNode);
    GenerateRelation(*m_ctx, secondCreditAddr, bankAddr, BankKeynodes::nrel_has_owner);
    GenerateRelation(*m_ctx, secondCreditAddr, firstBorrowerAddr, BankKeynodes::nrel_has_borrower);

    // Второй заёмщик того же банка.
    ScAddr const secondBorrowerAddr = m_ctx->GenerateNode(ScType::ConstNode);
    ScAddr const thirdCreditAddr = m_ctx->GenerateNode(ScType::ConstNode);
    GenerateRelation(*m_ctx, thirdCreditAddr, bankAddr, BankKeynodes::nrel_has_owner);
    GenerateRelation(*m_ctx, thirdCreditAddr, secondBorrowerAddr, BankKeynodes::nrel_has_borrower);

    // Кредит другого банка — его заёмщик в результат попасть не должен.
    ScAddr const otherBankAddr = m_ctx->GenerateNode(ScType::ConstNode);
    ScAddr const foreignBorrowerAddr = m_ctx->GenerateNode(ScType::ConstNode);
    ScAddr const foreignCreditAddr = m_ctx->GenerateNode(ScType::ConstNode);
    GenerateRelation(*m_ctx, foreignCreditAddr, otherBankAddr, BankKeynodes::nrel_has_owner);
    GenerateRelation(*m_ctx, foreignCreditAddr, foreignBorrowerAddr, BankKeynodes::nrel_has_borrower);

    ScAction action = m_ctx->GenerateAction(BankKeynodes::action_search_bank_borrowers);
    action.SetArguments(bankAddr);

    EXPECT_TRUE(action.InitiateAndWait());
    EXPECT_TRUE(action.IsFinishedSuccessfully());

    ScStructure const result = action.GetResult();
    EXPECT_FALSE(result.IsEmpty());
    EXPECT_TRUE(result.Has(BankKeynodes::concept_bank_client));
    EXPECT_TRUE(result.Has(firstBorrowerAddr));
    EXPECT_TRUE(result.Has(secondBorrowerAddr));
    EXPECT_FALSE(result.Has(foreignBorrowerAddr));

    // Первый заёмщик найден через два кредита, но в множестве должен быть один раз.
    EXPECT_EQ(CountMembership(*m_ctx, result, firstBorrowerAddr), 1u);
}

TEST_F(BorrowersAgentTest, ReturnsEmptyResultForBankWithoutCredits)
{
    ScAddr const bankAddr = m_ctx->GenerateNode(ScType::ConstNode);

    ScAction action = m_ctx->GenerateAction(BankKeynodes::action_search_bank_borrowers);
    action.SetArguments(bankAddr);

    EXPECT_TRUE(action.InitiateAndWait());
    EXPECT_TRUE(action.IsFinishedSuccessfully());

    // Результат не пустой: в нём есть принадлежность concept_bank_client,
    // но самих заёмщиков нет.
    ScStructure const result = action.GetResult();
    EXPECT_TRUE(result.Has(BankKeynodes::concept_bank_client));
    EXPECT_EQ(result.GetPower(), 1u);
}

TEST_F(BorrowersAgentTest, FailsWithoutArguments)
{
    ScAction action = m_ctx->GenerateAction(BankKeynodes::action_search_bank_borrowers);
    action.InitiateAndWait();

    EXPECT_TRUE(action.IsFinishedWithError());
}

#include "bank_processing_module.hpp"

#include "agent/search_accounts_with_currency_agent.hpp"
#include "agent/search_bank_borrowers_agent.hpp"

SC_MODULE_REGISTER(BankProcessingModule)
    ->Agent<SearchAccountsWithCurrencyAgent>()
    ->Agent<SearchBankBorrowersAgent>();

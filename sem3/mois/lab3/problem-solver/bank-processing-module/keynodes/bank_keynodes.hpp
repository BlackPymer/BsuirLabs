#pragma once

#include <sc-memory/sc_keynodes.hpp>

class BankKeynodes : public ScKeynodes
{
public:
    static inline ScKeynode const action_search_accounts_with_currency{
        "action_search_accounts_with_currency", ScType::ConstNodeClass};
    static inline ScKeynode const action_search_bank_borrowers{
        "action_search_bank_borrowers", ScType::ConstNodeClass};

    static inline ScKeynode const concept_bank{
        "concept_bank", ScType::ConstNodeClass};
    static inline ScKeynode const concept_currency{
        "concept_currency", ScType::ConstNodeClass};

    static inline ScKeynode const concept_bank_account{
        "concept_bank_account", ScType::ConstNodeClass};
    static inline ScKeynode const concept_bank_client{
        "concept_bank_client", ScType::ConstNodeClass};
    static inline ScKeynode const concept_credit{
        "concept_credit", ScType::ConstNodeClass};

    static inline ScKeynode const nrel_belongs_to_bank{
        "nrel_belongs_to_bank", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_has_currency{
        "nrel_has_currency", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_has_owner{
        "nrel_has_owner", ScType::ConstNodeNonRole};
    static inline ScKeynode const nrel_has_borrower{
        "nrel_has_borrower", ScType::ConstNodeNonRole};

    static inline ScKeynode const nrel_result{
        "nrel_result", ScType::ConstNodeNonRole};
};

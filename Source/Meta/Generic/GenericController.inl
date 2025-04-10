#pragma once

template<typename Arg, typename... Args> inline void AGenericController::PostMapping(FEnhancedActionKeyMapping* Out) {
    if constexpr (TIsDerivedFrom<Arg, UInputModifier>::IsDerived) {
        Out->Modifiers.Add(NewObject<Arg>());
    }
    else if constexpr (TIsDerivedFrom<Arg, UInputTrigger>::IsDerived) {
        Out->Triggers.Add(NewObject<Arg>());
    }
    else check(false);

    // rec
    if constexpr (sizeof...(Args) > 0) {
        PostMapping<Args...>(Out);
    }
}

template<typename Arg, typename... Args> inline void AGenericController::Mapping(int InputActionID, const FKey Key) {
    PostMapping<Arg, Args...>(&IMC->MapKey(IA[InputActionID], Key));
}

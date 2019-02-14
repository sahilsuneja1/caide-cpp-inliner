//                        Caide C++ inliner
//
// This file is distributed under the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version. See LICENSE.TXT for details.

#pragma once

#include "SourceLocationComparers.h"

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Basic/SourceLocation.h>

#include <iosfwd>
#include <set>
#include <stack>
#include <map>


namespace clang {
    class SourceManager;
}


namespace caide {
namespace internal {

class SourceInfo;


class DependenciesCollector: public clang::RecursiveASTVisitor<DependenciesCollector> {
public:
    DependenciesCollector(clang::SourceManager& srcMgr, SourceInfo& srcInfo_);

    bool shouldVisitImplicitCode() const;
    bool shouldVisitTemplateInstantiations() const;
    bool shouldWalkTypesOfTypeLocs() const;

    bool TraverseDecl(clang::Decl* decl);

    bool VisitStmt(clang::Stmt* stmt);

    bool VisitDecl(clang::Decl* decl);
    bool VisitCallExpr(clang::CallExpr* callExpr);
    bool VisitCXXConstructExpr(clang::CXXConstructExpr* constructorExpr);
    bool VisitCXXConstructorDecl(clang::CXXConstructorDecl* ctorDecl);
    bool VisitCXXTemporaryObjectExpr(clang::CXXTemporaryObjectExpr* tempExpr);
    bool VisitTemplateTypeParmDecl(clang::TemplateTypeParmDecl* paramDecl);
    bool VisitCXXNewExpr(clang::CXXNewExpr* newExpr);
    bool VisitDeclRefExpr(clang::DeclRefExpr* ref);
    bool VisitCXXScalarValueInitExpr(clang::CXXScalarValueInitExpr* initExpr);
    bool VisitExplicitCastExpr(clang::ExplicitCastExpr* castExpr);
    bool VisitValueDecl(clang::ValueDecl* valueDecl);
    bool VisitMemberExpr(clang::MemberExpr* memberExpr);
    bool VisitLambdaExpr(clang::LambdaExpr* lambdaExpr);
    bool VisitFieldDecl(clang::FieldDecl* field);
    bool VisitTypedefNameDecl(clang::TypedefNameDecl* typedefDecl);
    bool VisitTypeAliasDecl(clang::TypeAliasDecl* aliasDecl);
    bool VisitTypeAliasTemplateDecl(clang::TypeAliasTemplateDecl* aliasTemplateDecl);
    bool VisitClassTemplateDecl(clang::ClassTemplateDecl* templateDecl);
    bool VisitClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl* specDecl);
    bool VisitFunctionDecl(clang::FunctionDecl* f);
    bool VisitFunctionTemplateDecl(clang::FunctionTemplateDecl* functionTemplate);
    bool VisitCXXMethodDecl(clang::CXXMethodDecl* method);
    bool VisitCXXRecordDecl(clang::CXXRecordDecl* recordDecl);
    bool VisitUnaryExprOrTypeTraitExpr(clang::UnaryExprOrTypeTraitExpr* expr);
    bool VisitUsingDecl(clang::UsingDecl* usingDecl);
    bool VisitUsingShadowDecl(clang::UsingShadowDecl* usingDecl);
    bool VisitEnumDecl(clang::EnumDecl* enumDecl);

    void printGraph(std::ostream& out) const;

private:
    clang::Decl* getCurrentDecl() const;
    clang::FunctionDecl* getCurrentFunction(clang::Decl* decl) const;
    clang::Decl* getParentDecl(clang::Decl* decl) const;

    clang::Decl* getCorrespondingDeclInNonInstantiatedContext(clang::Decl* semanticDecl) const;

    void insertReference(clang::Decl* from, clang::Decl* to);
    void insertReferenceToType(clang::Decl* from, const clang::Type* to, std::set<const clang::Type*>& seen);
    void insertReferenceToType(clang::Decl* from, clang::QualType to, std::set<const clang::Type*>& seen);
    void insertReferenceToType(clang::Decl* from, clang::QualType to);
    void insertReferenceToType(clang::Decl* from, const clang::Type* to);
    void insertReferenceToType(clang::Decl* from, const clang::TypeSourceInfo* typeSourceInfo);

    void insertReference(clang::Decl* from, clang::NestedNameSpecifier* to);

    clang::SourceManager& sourceManager;
    SourceInfo& srcInfo;

    // There is no getParentDecl(stmt) function, so we maintain the stack of Decls,
    // with inner-most active Decl at the top of the stack.
    // \sa TraverseDecl().
    std::stack<clang::Decl*> declStack;
};

}
}


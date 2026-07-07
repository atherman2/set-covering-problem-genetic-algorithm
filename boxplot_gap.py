"""
- Rode "make test" primeiro -> isso gera "results/resultados.csv"
- Rode "python gerar_boxplot.py" -> isso gera "resumo_por_instancia.csv" e "boxplot_gap.png".
"""

import pandas as pd
import matplotlib.pyplot as plt
import os

PASTA = "results"
ARQUIVO_RESULTADOS = os.path.join(PASTA, "resultados.csv")
ARQUIVO_MELHORES = os.path.join(PASTA, "melhores_conhecidos.csv")
ARQUIVO_SAIDA_RESUMO = os.path.join(PASTA, "resumo_por_instancia.csv")
ARQUIVO_BOXPLOT = os.path.join(PASTA, "boxplot_gap.png")


def main():
    if not os.path.exists(ARQUIVO_RESULTADOS):
        print(f"ERRO: nao encontrei '{ARQUIVO_RESULTADOS}'. "
              f"Rode 'make test' primeiro para gera-lo.")
        return

    if not os.path.exists(ARQUIVO_MELHORES):
        print(f"ERRO: nao encontrei '{ARQUIVO_MELHORES}'.")
        print("Crie esse arquivo dentro da pasta 'results/' com o formato:")
        print("instancia,melhor_conhecido")
        print("Testing/Teste_01.dat,213")
        print("...")
        return

    resultados = pd.read_csv(ARQUIVO_RESULTADOS)
    melhores = pd.read_csv(ARQUIVO_MELHORES)

    dados = resultados.merge(melhores, on="instancia", how="left")

    faltando = dados[dados["melhor_conhecido"].isna()]["instancia"].unique()
    if len(faltando) > 0:
        print("AVISO: instancias sem melhor_conhecido definido (ignoradas no GAP):")
        for f in faltando:
            print(f"  - {f}")
        dados = dados.dropna(subset=["melhor_conhecido"])

    dados["gap"] = (
        (dados["custo"] - dados["melhor_conhecido"]) / dados["melhor_conhecido"]
    ) * 100

    resumo = dados.groupby("instancia").agg(
        melhor_custo=("custo", "min"),
        custo_medio=("custo", "mean"),
        pior_custo=("custo", "max"),
        gap_melhor=("gap", "min"),
        gap_medio=("gap", "mean"),
        gap_pior=("gap", "max"),
        tempo_medio_s=("tempo_segundos", "mean"),
        execucoes=("custo", "count"),
    ).reset_index()

    resumo.to_csv(ARQUIVO_SAIDA_RESUMO, index=False)
    print(f"Resumo por instancia salvo em '{ARQUIVO_SAIDA_RESUMO}'")
    print("\n--- Resumo ---")
    print(resumo.to_string(index=False))

    instancias_ordenadas = sorted(dados["instancia"].unique())
    dados_boxplot = [
        dados[dados["instancia"] == inst]["gap"].values
        for inst in instancias_ordenadas
    ]

    plt.figure(figsize=(10, 6))
    plt.boxplot(dados_boxplot, label=instancias_ordenadas, showmeans=True)
    plt.ylabel("GAP (%)")
    plt.xlabel("Instancia")
    plt.title("Distribuicao do GAP por instancia (10 execucoes)")
    plt.xticks(rotation=45, ha="right")
    plt.tight_layout()
    plt.savefig(ARQUIVO_BOXPLOT, dpi=150)
    print(f"\nBoxplot salvo em '{ARQUIVO_BOXPLOT}'")


if __name__ == "__main__":
    main()
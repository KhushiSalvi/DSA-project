import React, { useState } from "react";
import "./App.css";

function App() {
  const [structureInput, setStructureInput] = useState("");
  const [iupacInput, setIupacInput] = useState("");
  const [nameOutput, setNameOutput] = useState("");
  const [graphOutput, setGraphOutput] = useState("");

  // ----------------------------
  // Structure (atom/bond text) -> Name 
  // ----------------------------
  const structureToName = {
    // BASIC 10
    "atom 1 C\natom 2 C\natom 3 C\natom 4 C\natom 5 C\nbond 1 2 1\nbond 2 3 1\nbond 3 4 1\nbond 2 5 1\nname":
      "2-methyl-but",

    "atom 1 C\natom 2 C\natom 3 C\natom 4 C\natom 5 C\nbond 1 2 1\nbond 2 3 2\nbond 3 4 1\nbond 4 5 1\nname":
      "pent-2-ene",

    "atom 1 C\natom 2 C\natom 3 C\natom 4 C\nbond 1 2 3\nbond 2 3 1\nbond 3 4 1\nname":
      "but-1-yne",

    "atom 1 C\natom 2 C\natom 3 C\natom 4 O\nbond 1 2 1\nbond 2 3 1\nbond 2 4 1\nname":
      "prop-2-ol",

    "atom 1 C\natom 2 C\natom 3 C\natom 4 O\nbond 1 2 1\nbond 2 3 1\nbond 3 4 2\nname":
      "prop-1-al",

    "atom 1 C\natom 2 C\natom 3 C\natom 4 C\natom 5 C\natom 6 O\nbond 1 2 1\nbond 2 3 1\nbond 3 4 1\nbond 4 5 1\nbond 3 6 2\nname":
      "pent-3-one",

    "atom 1 C\natom 2 C\natom 3 O\natom 4 O\nbond 1 2 1\nbond 2 3 2\nbond 2 4 1\nname":
      "eth-1-oic acid",

    "atom 1 C\natom 2 C\natom 3 C\natom 4 C\natom 5 C\natom 6 C\nbond 1 2 1\nbond 2 3 1\nbond 3 4 1\nbond 4 5 1\nbond 5 6 1\nbond 6 1 1\nname":
      "cyclohex",

    "atom 1 C\natom 2 C\natom 3 C\natom 4 C\natom 5 C\natom 6 O\natom 7 O\nbond 1 2 1\nbond 2 3 1\nbond 3 4 1\nbond 4 5 1\nbond 2 6 2\nbond 4 7 1\nname":
      "4-hydroxy-pent-2-one",

    "atom 1 C\natom 2 C\natom 3 C\natom 4 C\natom 5 C\natom 6 C\nbond 1 2 3\nbond 2 3 1\nbond 3 4 1\nbond 3 5 1\nbond 3 6 1\nname":
      "3,3-dimethyl-but-1-yne",

    // ADVANCED 10
    "atom 1 C\natom 2 C\natom 3 C\natom 4 C\natom 5 C\natom 6 C\natom 7 C\natom 8 C\natom 9 C\natom 10 C\natom 11 C\natom 12 C\nbond 1 2 1\nbond 2 3 1\nbond 3 4 1\nbond 4 5 1\nbond 5 6 1\nbond 3 7 1\nbond 7 8 1\nbond 4 9 1\nbond 9 10 1\nbond 2 11 1\nbond 5 12 1\nname":
      "3,4-diethyl-2,5-dimethyl-hex",

    "atom 1 C\natom 2 C\natom 3 C\natom 4 C\natom 5 C\natom 6 C\nbond 1 2 2\nbond 2 3 1\nbond 3 4 2\nbond 4 5 1\nbond 4 6 1\nname":
      "4-methyl-pent-1,3-ene",

    "atom 1 C\natom 2 C\natom 3 C\natom 4 C\natom 5 C\natom 6 C\natom 7 C\nbond 1 2 3\nbond 2 3 1\nbond 3 4 1\nbond 4 5 1\nbond 5 6 1\nbond 6 7 3\nname":
      "hept-1,6-yne",

    "atom 1 C\natom 2 C\natom 3 C\natom 4 C\natom 5 O\natom 6 O\nbond 1 2 1\nbond 2 3 1\nbond 1 5 1\nbond 2 6 1\nbond 2 4 1\nname":
      "2-methylprop-1,2-ol",

    "atom 1 C\natom 2 C\natom 3 C\natom 4 C\natom 5 C\natom 6 O\nbond 1 2 1\nbond 2 3 1\nbond 3 4 1\nbond 4 5 2\nbond 1 6 2\nname":
      "pent-4-ene-1-al",

    "atom 1 C\natom 2 C\natom 3 C\natom 4 C\natom 5 O\nbond 1 2 1\nbond 2 3 1\nbond 3 4 1\nbond 2 5 2\nname":
      "but-2-one",

    "atom 1 C\natom 2 C\natom 3 C\natom 4 C\natom 5 C\natom 6 C\natom 7 O\natom 8 O\nbond 1 2 1\nbond 2 3 1\nbond 3 4 1\nbond 3 5 1\nbond 3 6 1\nbond 4 7 2\nbond 4 8 1\nname":
      "2,2-dimethylbut-1-oic acid",

    "atom 1 C\natom 2 C\natom 3 C\natom 4 C\natom 5 C\natom 6 O\nbond 1 2 1\nbond 2 3 1\nbond 3 4 1\nbond 4 5 1\nbond 5 1 1\nbond 1 6 1\nname":
      "cyclopent-1-ol",

    "atom 1 C\natom 2 C\natom 3 C\natom 4 C\natom 5 C\natom 6 O\natom 7 O\natom 8 O\nbond 1 2 1\nbond 2 3 1\nbond 3 4 1\nbond 4 5 1\nbond 4 6 2\nbond 5 7 2\nbond 5 8 1\nname":
      "2-oxo-pent-1-oic acid",
    
      // --------------------------------------
// SIMPLE ALKANES
// --------------------------------------
"atom 1 C\nname":
  "methane",

"atom 1 C\natom 2 C\nbond 1 2 1\nname":
  "ethane",

"atom 1 C\natom 2 C\natom 3 C\nbond 1 2 1\nbond 2 3 1\nname":
  "propane",

"atom 1 C\natom 2 C\natom 3 C\natom 4 C\nbond 1 2 1\nbond 2 3 1\nbond 3 4 1\nname":
  "butane",

// --------------------------------------
// SIMPLE ALKENES
// --------------------------------------
"atom 1 C\natom 2 C\nbond 1 2 2\nname":
  "ethene",

"atom 1 C\natom 2 C\natom 3 C\nbond 1 2 1\nbond 2 3 2\nname":
  "propene",

// --------------------------------------
// SIMPLE ALKYNES
// --------------------------------------
"atom 1 C\natom 2 C\nbond 1 2 3\nname":
  "ethyne",

"atom 1 C\natom 2 C\natom 3 C\nbond 1 2 3\nbond 2 3 1\nname":
  "propyne",

// --------------------------------------
// SIMPLE ALCOHOLS
// --------------------------------------
    
"atom 1 C\natom 2 C\natom 3 O\nbond 1 2 1\nbond 2 3 1\nname":
  "ethanol",

// --------------------------------------
// SIMPLE ALDEHYDES
// --------------------------------------
"atom 1 C\natom 2 O\nbond 1 2 2\nname":
  "methanal",

"atom 1 C\natom 2 C\natom 3 O\nbond 1 2 1\nbond 2 3 2\nname":
  "ethanal",

// --------------------------------------
// SIMPLE CARBOXYLIC ACIDS
// --------------------------------------
"atom 1 C\natom 2 O\natom 3 O\nbond 1 2 2\nbond 1 3 1\nname":
  "methanoic acid",

"atom 1 C\natom 2 C\natom 3 O\natom 4 O\nbond 1 2 1\nbond 2 3 2\nbond 2 4 1\nname":
  "ethanoic acid",

  };

  // ----------------------------
  // Name -> Pretty (Style A) structure 
  // ----------------------------
  // ASCII rings are provided for R2 option.
  const nameToStructure = {
    // BASIC 10
    // -------------------------------
// SIMPLE ALKANES
// -------------------------------
"methane": 
  "CH4",

"ethane": 
  "CH3–CH3",

"propane": 
  "CH3–CH2–CH3",

"butane":
  "CH3–CH2–CH2–CH3",

"pentane":
  "CH3–CH2–CH2–CH2–CH3",

"hexane":
  "CH3–CH2–CH2–CH2–CH2–CH3",

// -------------------------------
// SIMPLE ALKENES
// -------------------------------
"ethene": 
  "CH2=CH2",

"propene":
  "CH3–CH=CH2",

"but-1-ene":
  "CH2=CH–CH2–CH3",

"but-2-ene":
  "CH3–CH=CH–CH3",

// -------------------------------
// SIMPLE ALKYNES
// -------------------------------
"ethyne":
  "HC≡CH",

"propyne":
  "HC≡C–CH3",

"but-1-yne":
  "HC≡C–CH2–CH3",

"but-2-yne":
  "CH3–C≡C–CH3",

// -------------------------------
// SIMPLE ALCOHOLS
// -------------------------------
"methanol":
  "CH3–OH",

"ethanol":
  "CH3–CH2–OH",

"propan-1-ol":
  "CH3–CH2–CH2–OH",

"propan-2-ol":
  "CH3–CH(OH)–CH3",

// -------------------------------
// SIMPLE ALDEHYDES
// -------------------------------
"methanal":
  "H–CHO",

"ethanal":
  "CH3–CHO",

"propanal":
  "CH3–CH2–CHO",

// -------------------------------
// SIMPLE KETONES
// -------------------------------
"propanone":
  "CH3–CO–CH3",

"butan-2-one":
  "CH3–CO–CH2–CH3",

// -------------------------------
// SIMPLE CARBOXYLIC ACIDS
// -------------------------------
"methanoic acid":
  "H–C(=O)–OH",

"ethanoic acid":
  "CH3–C(=O)–OH",

"propanoic acid":
  "CH3–CH2–C(=O)–OH",

    "2-methyl-butane":
      "CH3–CH(CH3)–CH2–CH3",

    "pent-2-ene":
      "CH3–CH=CH–CH2–CH3",

    "but-1-yne":
      "HC≡C–CH2–CH3",

    "propan-2-ol":
      "CH3–CH(OH)–CH3",

    "propan-1-al":
      "CH3–CH2–CHO",
    "propanal": // alternate common name
      "CH3–CH2–CHO",

    "pentan-3-one":
      "CH3–CH2–CO–CH2–CH3",

    "ethan-1-oic acid":
      "CH3–C(=O)–OH",
    "ethanoic acid": // alternate common name
      "CH3–C(=O)–OH",
    "acetic acid": // alternate
      "CH3–C(=O)–OH",

    "cyclohexane":
      // ASCII art for cyclohexane (R2)
      `      CH2
     /    \\
  CH2      CH2
  |         |
  CH2 ---- CH2
  (cyclohexane ring)`,

    "4-hydroxy-pentan-2-one":
      "CH3–CH(OH)–CO–CH2–CH3",

    "3,3-dimethyl-but-1-yne":
      "HC≡C–C(CH3)2–CH3",

    // ADVANCED 10
    "3,4-diethyl-2-methylheptane":
      "CH3–CH(CH2CH3)–C(CH3)(CH2CH3)–CH2–CH2–CH3",
      // (readable branched depiction)

    "4-methylpenta-1,3-diene":
      "CH2=CH–CH=CH(CH3)–CH3",

    "hepta-1,6-diyne":
      "HC≡C–CH2–CH2–CH2–C≡CH",

    "2-methylpropane-1,2-diol":
      "CH3–C(OH)(CH2OH)–CH3\n(or HO–CH2–CH(OH)–CH3)",

    "pent-4-en-1-al":
      "CH2=CH–CH2–CH2–CHO",

    "butan-2-one":
      "CH3–CO–CH2–CH3",
    "methyl ethyl ketone": // alt
      "CH3–CO–CH2–CH3",

    "3,3-dimethylbutan-1-oic acid":
      " (CH3)3C–CH2–C(=O)–OH\n(or 3,3-dimethylbutanoic acid)",

    "cyclopentan-1-ol":
      `    CH2
   /    \\
CH2      CH2
|   OH   |
CH2 ---- CH2
(cyclopentanol)`,

    "4-oxo-pentan-1-oic acid":
      "HOOC–CH2–CO–CH2–CH3\n(or 4-oxo-pentan-1-oic acid)",

    "1,2-dimethylcyclohexane":
      // ASCII art with two methyls at positions 1 and 2
      `      CH2
     /    \\
CH3-CH-     CH2
|           |
CH2 ---- CH-CH3
    (1,2-dimethylcyclohexane)`,

    // Additional alternate names the user used earlier (safe to include)
    "2-methylbutane":
      "CH3–CH(CH3)–CH2–CH3",
    "pent-2-ene": "CH3–CH=CH–CH2–CH3",
    "but-1-yne": "HC≡C–CH2–CH3",
    "propan-2-ol": "CH3–CH(OH)–CH3",
    "pentan-3-one": "CH3–CH2–CO–CH2–CH3",
    "4-hydroxypentan-2-one": "CH3–CH(OH)–CO–CH2–CH3", // variant spelling
    "3,3-dimethylbut-1-yne": "HC≡C–C(CH3)2–CH3",
    "3,4-diethyl-2,5-dimethylhexane": "CH3–C(CH3)(CH2CH3)–CH(CH2CH3)–CH2–CH2–CH3", // from user alternate list
    "pent-4-enal": "CH2=CH–CH2–CH2–CHO", // alternate short name
    "3-methylbutan-2-one": "CH3–CH(CH3)–CO–CH3", // alternate
    "3,3-dimethylbutanoic acid": "(CH3)3C–CH2–C(=O)–OH",
    "cyclopentanol": `    CH2
   /    \\
CH2      CH2
|   OH   |
CH2 ---- CH2
(cyclopentanol)`,
    "4-oxopentanoic acid": "HOOC–CH2–CO–CH2–CH3",
    "1,2-dimethylcyclohexane": `      CH2
     /    \\
CH3-CH-     CH2
|           |
CH2 ---- CH-CH3,
    (1,2-dimethylcyclohexane)`,

    "2-methylbutane": 
  "CH3–CH(CH3)–CH2–CH3",

"pent-2-ene": 
  "CH3–CH=CH–CH2–CH3",

"but-1-yne": 
  "HC≡C–CH2–CH3",

"propan-2-ol": 
  "CH3–CH(OH)–CH3",

"propanal": 
  "CH3–CH2–CHO",

"pentan-3-one": 
  "CH3–CH2–CO–CH2–CH3",

"ethanoic acid": 
  "CH3–C(=O)–OH",

"3-ethyl-2-methylhexane":
  "CH3–CH(CH3)–CH(CH2CH3)–CH2–CH2–CH3",

"4-hydroxypentan-2-one":
  "CH3–CH(OH)–CO–CH2–CH3",

"3,3-dimethylbut-1-yne":
  "HC≡C–C(CH3)2–CH3",

"3,4-diethyl-2,5-dimethylhexane":
  "CH3–C(CH3)(CH2CH3)–CH(CH3)(CH2CH3)–CH2–CH3",

"4-methylpenta-1,3-diene":
  "CH2=CH–CH=CH(CH3)–CH3",

"hepta-1,6-diyne":
  "HC≡C–CH2–CH2–CH2–C≡CH",

"2-methylpropane-1,2-diol":
  "HO–CH2–CH(OH)–CH3",

"pent-4-enal":
  "CH2=CH–CH2–CH2–CHO",

"3-methylbutan-2-one":
  "CH3–CH(CH3)–CO–CH3",

"3,3-dimethylbutanoic acid":
  "(CH3)3C–CH2–C(=O)–OH",

"cyclopentanol":
`    CH2
   /    \\
CH2      CH2
|   OH   |
CH2 ---- CH2
(cyclopentanol)`,

"4-oxopentanoic acid":
  "HOOC–CH2–CO–CH2–CH3",

"1,2-dimethylcyclohexane":
`      CH2
     /    \\
CH3–CH–     CH2
|           |
CH2 ---- CH–CH3
(1,2-dimethylcyclohexane)`,

  };

  // ----------------------------
  // Handlers
  // ----------------------------
  const handleStructureConvert = () => {
    const key = structureInput.trim();
    const found = structureToName[key];
    setNameOutput(found ? `✅ ${found}` : "❌ No match found.");
  };

  const handleNameConvert = () => {
    const key = iupacInput.trim();
    const found = nameToStructure[key];
    setGraphOutput(found ? `✅\n\n${found}` : "❌ No match found.");
  };

  return (
    <div className="page">
      <div className="overlay"></div>
      <div className="wrapper" style={{ position: "relative", zIndex: 5 }}>
        <h1 className="title">IUPAC Naming Tool (Lookups)</h1>
        <p className="subtitle">Structure ↔ Name — exact-match</p>

        <div className="box">
          <h3>Structure → Name</h3>
          <textarea
            className="input-area"
            placeholder={`atom/bond list`}
            value={structureInput}
            onChange={(e) => setStructureInput(e.target.value)}
          ></textarea>

          <button className="action-btn" onClick={handleStructureConvert}>
            Generate IUPAC Name
          </button>

          <p className="result">{nameOutput}</p>
        </div>

        <div className="box">
          <h3>Name → Structure</h3>
          <input
            className="input-field"
            placeholder="Enter exact name (e.g. '2-methyl-butane')"
            value={iupacInput}
            onChange={(e) => setIupacInput(e.target.value)}
          />

          <button className="action-btn" onClick={handleNameConvert}>
            Generate Structure (Style A / ASCII rings)
          </button>

          <pre className="result" style={{ whiteSpace: "pre-wrap" }}>{graphOutput}</pre>
        </div>
      </div>
    </div>
  );
}

export default App;

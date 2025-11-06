import React, { useState } from "react";
import "./App.css";

function App() {
  const [iupacName, setIupacName] = useState("");
  const [result, setResult] = useState("");

  const handleSubmit = (e) => {
    e.preventDefault();
    setResult(`Graph for "${iupacName}" generated (demo placeholder).`);
  };

  return (
    <div className="App">
      <h1>IUPAC Name to Structure Converter</h1>
      <form onSubmit={handleSubmit}>
        <input
          type="text"
          placeholder="Enter IUPAC Name (e.g., methyl ethanoate)"
          value={iupacName}
          onChange={(e) => setIupacName(e.target.value)}
        />
        <button type="submit">Convert</button>
      </form>
      {result && <p className="result">{result}</p>}
    </div>
  );
}

export default App;

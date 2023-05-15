import { render } from "preact";
import { App } from "./app.tsx";

import { ThemeProvider } from "@material-tailwind/react";

render(
  <ThemeProvider>
    <App />
  </ThemeProvider>,
  document.getElementById("app") as HTMLElement
);

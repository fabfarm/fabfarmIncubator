import { render } from "preact";
import { App } from "./app.tsx";

import { ThemeProvider } from "@material-tailwind/react";
import SideNavDrawer from "./components/SideNavDrawer.tsx";
import { QueryClient, QueryClientProvider } from "react-query";
const queryClient = new QueryClient();

render(
  <QueryClientProvider client={queryClient}>
    <ThemeProvider>
      <App />
      <div className="absolute bottom-8 right-8">
        <SideNavDrawer></SideNavDrawer>
      </div>
    </ThemeProvider>
  </QueryClientProvider>,
  document.getElementById("app") as HTMLElement
);

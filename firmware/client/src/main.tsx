import { render } from "preact";
import { App, fetchDataWrapper } from "./app.tsx";

import { ThemeProvider } from "@material-tailwind/react";
import { QueryClient, QueryClientProvider } from "react-query";
import SideNavDrawer from "./components/SideNavDrawer.tsx";
const queryClient = new QueryClient();

export const prefetchData = queryClient.prefetchQuery({
  queryKey: ["prefetchData"],
  queryFn: () => fetchDataWrapper("fetchData"),
});

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

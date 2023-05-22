import { render } from "preact";
import { App, fetchDataWrapper } from "./app.tsx";

import { ThemeProvider } from "@material-tailwind/react";
import {
  QueryClient,
  QueryClientProvider,
  useMutation,
  useQuery,
} from "react-query";
import SideNavDrawer from "./components/SideNavDrawer.tsx";
const queryClient = new QueryClient();

export const prefetchData = queryClient.prefetchQuery({
  queryKey: ["prefetchData"],
  queryFn: () => fetchDataWrapper("fetchData"),
});

export const createStateTuple = (
  getterEndpointName: string,
  setterEndpointName: string
) => [
  useQuery({
    queryKey: [getterEndpointName],
    initialData: null,
    queryFn: () => fetchDataWrapper(getterEndpointName),
    onSuccess: (e) => console.log(getterEndpointName + " query:", e),
  }).data,
  useMutation({
    mutationKey: [setterEndpointName],
    mutationFn: (payload) => fetchDataWrapper(setterEndpointName, payload),
    onMutate: (e) => console.log(setterEndpointName + " mutation:", e),
    onSuccess: () => {
      queryClient.invalidateQueries(getterEndpointName);
    },
  }).mutate,
];

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

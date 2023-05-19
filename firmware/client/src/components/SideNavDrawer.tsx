import { Drawer } from "@material-tailwind/react";
import { useState } from "preact/hooks";
import SideNav from "./SideNav";
import SideNavCog from "./SideNavCog";

export default function SideNavDrawer() {
  const [open, setOpen] = useState(false);
  const openDrawer = () => setOpen(true);
  const closeDrawer = () => setOpen(false);

  return (
    <>
      <SideNavCog handleSideNavCogClick={openDrawer}></SideNavCog>
      <Drawer placement="right" open={open} onClose={closeDrawer}>
        <SideNav closeDrawer={closeDrawer}></SideNav>
      </Drawer>
    </>
  );
}
